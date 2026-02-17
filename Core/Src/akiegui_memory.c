/* ============= akiegui_memory.c ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 内存管理器实现
 * 
 * 实现内存池管理：
 *   - 首次适应算法
 *   - 16字节默认对齐
 *   - 魔数校验防野指针
 *   - 临界区保护（通过port层）
 * 
 * 支持裸机内存池和FreeRTOS堆两种模式
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#include "akiegui_memory.h"
#include "stdint.h"
#include "stm32h7xx.h"
#include <string.h>

#if !AkieGUI_USE_FREERTOS
static AkieGUI_Mem_T g_mem;
static uint8_t g_mem_inited = 0;
#endif

int AkieGUI_MemInit(void *start, uint32_t size) {
    #if AkieGUI_USE_FREERTOS
    /* FreeRTOS 版本：忽略参数，使用FreeRTOS堆 */
    (void)start;
    (void)size;
    
    /* 检查FreeRTOS堆是否已初始化 */
    #ifdef xPortGetFreeHeapSize
    if (xPortGetFreeHeapSize() == 0) {
        return -1;  /* FreeRTOS堆未初始化 */
    }
    #endif
    return 0;
#else
    uint32_t primask;  /* 保存中断状态 */
    /* 裸机版本：原有初始化代码 */
    if (g_mem_inited) return 0;
    if (!start || size < sizeof(AkieGUI_Mem_Block_T) + AkieGUI_ALIGN) {
        return -1;  /* 内存池太小 */
    }

    AkieGUI_ENTER_CRITICAL(primask);
    
    g_mem.pool_start = (uint8_t*)start;
    g_mem.pool_size = size;
    g_mem.free_size = size;
    g_mem.block_count = 0;
    
    g_mem.free_list = (AkieGUI_Mem_Block_T*)start;
    g_mem.free_list->next = NULL;
    g_mem.free_list->size = size - sizeof(AkieGUI_Mem_Block_T);
    g_mem.free_list->used = 0;
    g_mem.free_list->magic = 0xDEADBEEF;
    
    g_mem_inited = 1;
    AkieGUI_EXIT_CRITICAL(primask);
    return 0;
#endif
}

void* AkieGUI_MemAlloc(uint32_t size) {
#if AkieGUI_USE_FREERTOS
    void *ptr;
    ptr = pvPortMalloc(size);
    return ptr;
#else
    return AkieGUI_MemAllocAlign(size, AkieGUI_ALIGN);
#endif
}

void* AkieGUI_MemAllocAlign(uint32_t size, uint32_t align) {
  uint32_t primask;  /* 保存中断状态 */

    if (size == 0) return NULL;
    if (align == 0 || (align & (align - 1)) != 0) {
        align = AkieGUI_ALIGN;  /* 回退到默认 */
    }
#if AkieGUI_USE_FREERTOS
    void *ptr;
    AkieGUI_ENTER_CRITICAL(primask);
    ptr = pvPortMalloc(size + align);
    if (ptr) {
        uintptr_t addr = (uintptr_t)ptr;
        uintptr_t aligned = (addr + align - 1) & ~(align - 1);
        if (addr != aligned) {
            uintptr_t *p = (uintptr_t*)aligned - 1;
            *p = aligned - addr;
            ptr = (void*)aligned;
        }
    }
    AkieGUI_EXIT_CRITICAL(primask);
    return ptr;
#else
    if (!g_mem_inited || size == 0) return NULL;
    
    /* ===== 进入临界区！保护链表操作 ===== */
    AkieGUI_ENTER_CRITICAL(primask);
    
    size = AkieGUI_ALIGN_UP(size, align);
    uint32_t total_size = size + sizeof(AkieGUI_Mem_Block_T) + align;
    
    AkieGUI_Mem_Block_T *prev = NULL;
    AkieGUI_Mem_Block_T *curr = g_mem.free_list;
    
    /* 遍历空闲链表 */
    while (curr) {
        if (!curr->used && curr->size >= total_size) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    
    if (!curr) {
        AkieGUI_EXIT_CRITICAL(primask);  /* 退出临界区 */
        return NULL;
    }
    
    /* 对齐计算 */
    uint8_t *data_ptr = (uint8_t*)curr + sizeof(AkieGUI_Mem_Block_T);
    uint8_t *aligned_ptr = (uint8_t*)(((uint32_t)data_ptr + align - 1) & ~(align - 1));
    uint32_t offset = aligned_ptr - data_ptr;
    
    void *result = NULL;
    
    if (offset > 0) {
        /* 需要对齐填充，创建新块 */
        AkieGUI_Mem_Block_T *new_block = 
            (AkieGUI_Mem_Block_T*)(aligned_ptr - sizeof(AkieGUI_Mem_Block_T));
        new_block->size = size;
        new_block->used = 1;
        new_block->magic = 0xFEEDBEEF;
        new_block->next = NULL;
        
        /* 调整原空闲块 */
        curr->size = aligned_ptr - (uint8_t*)curr - sizeof(AkieGUI_Mem_Block_T) * 2;
        curr->used = 0;
        
        g_mem.free_size -= (size + sizeof(AkieGUI_Mem_Block_T));
        g_mem.block_count++;
        
        result = (void*)(new_block + 1);
    } else {
        /* 直接使用当前块 */
        curr->used = 1;
        curr->size = size;
        curr->magic = 0xFEEDBEEF;
        
        /* 如果剩余空间足够，分割内存 */
        if (curr->size > size + sizeof(AkieGUI_Mem_Block_T) + AkieGUI_ALIGN) {
            AkieGUI_Mem_Block_T *next_block = 
                (AkieGUI_Mem_Block_T*)((uint8_t*)curr + sizeof(AkieGUI_Mem_Block_T) + size);
            next_block->next = curr->next;
            next_block->size = curr->size - size - sizeof(AkieGUI_Mem_Block_T);
            next_block->used = 0;
            next_block->magic = 0xDEADBEEF;
            
            if (prev) {
                prev->next = next_block;
            } else {
                g_mem.free_list = next_block;
            }
            
            curr->next = NULL;
            curr->size = size;
        }
        
        g_mem.free_size -= (curr->size + sizeof(AkieGUI_Mem_Block_T));
        g_mem.block_count++;
        
        result = (void*)(curr + 1);
    }
    
    /* ===== 退出临界区 ===== */
    AkieGUI_EXIT_CRITICAL(primask);
    return result;
#endif
}

void* AkieGUI_MemCalloc(uint32_t nmemb, uint32_t size) {
    uint32_t total = nmemb * size;
    void *ptr = AkieGUI_MemAlloc(total);
#if AkieGUI_USE_FREERTOS
    if (ptr) memset(ptr, 0, total);
    return ptr;
#else
    if (ptr) {
        uint32_t primask;  /* 保存中断状态 */
        AkieGUI_ENTER_CRITICAL(primask);
        memset(ptr, 0, total);
        AkieGUI_EXIT_CRITICAL(primask);
    }
#endif
    return ptr;
}


void AkieGUI_MemFree(void *ptr) {
    uint32_t primask;  /* 保存中断状态 */
#if AkieGUI_USE_FREERTOS
    if (!ptr) return;
    AkieGUI_ENTER_CRITICAL(primask);
    
    uintptr_t *p = (uintptr_t*)ptr;
    uintptr_t offset = p[-1];
    uintptr_t orig = (uintptr_t)ptr;
    if (offset > 0 && offset < 1024) {
        orig = (uintptr_t)ptr - offset;
    }
    vPortFree((void*)orig);
    
    AkieGUI_EXIT_CRITICAL(primask);
#else
    if (!ptr || !g_mem_inited) return;
    
    /* ===== 进入临界区！保护链表操作 ===== */
    AkieGUI_ENTER_CRITICAL(primask);
    
    AkieGUI_Mem_Block_T *block = (AkieGUI_Mem_Block_T*)ptr - 1;
    
    /* 验证魔数 */
    if (block->magic != 0xFEEDBEEF) {
        AkieGUI_EXIT_CRITICAL(primask);
        return;
    }
    
    /* 标记为空闲 */
    block->used = 0;
    block->magic = 0xDEADBEEF;
    g_mem.free_size += (block->size + sizeof(AkieGUI_Mem_Block_T));
    g_mem.block_count--;
    
    /* 插入空闲链表头部 */
    block->next = g_mem.free_list;
    g_mem.free_list = block;
    
    /* ===== 退出临界区 ===== */
    AkieGUI_EXIT_CRITICAL(primask);
#endif
}

uint32_t AkieGUI_MemGetFree(void) {
    uint32_t free_size;/* 空闲大小 */
    uint32_t primask;  /* 保存中断状态 */
#if AkieGUI_USE_FREERTOS
    AkieGUI_ENTER_CRITICAL(primask);
    free_size = xPortGetFreeHeapSize();
    AkieGUI_EXIT_CRITICAL(primask);
#else
    AkieGUI_ENTER_CRITICAL(primask);
    free_size = g_mem.free_size;
    AkieGUI_EXIT_CRITICAL(primask);
#endif
    return free_size;
}

uint32_t AkieGUI_MemGetUsed(void) {
#if AkieGUI_USE_FREERTOS
    #ifdef configTOTAL_HEAP_SIZE
        return configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize();
    #else
        return 0;
    #endif
#else
    uint32_t primask;  /* 保存中断状态 */
    uint32_t used_size;
    AkieGUI_ENTER_CRITICAL(primask);
    used_size = g_mem.pool_size - g_mem.free_size;
    AkieGUI_EXIT_CRITICAL(primask);
    return used_size;
#endif
}
