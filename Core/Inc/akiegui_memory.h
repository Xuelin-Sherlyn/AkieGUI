/* ============= akiegui_memory.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 内存管理器头文件
 * 
 * 定义内存管理的数据结构和API：
 *   - 内存块结构
 *   - 内存池管理器
 *   - 分配/释放函数声明
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_MEMORY_H__
#define __AKIEGUI_MEMORY_H__

#include "akiegui_config.h"
#include "akiegui_port.h"

/* 内存对齐 */
#ifndef AkieGUI_ALIGN
#define AkieGUI_ALIGN 16
#endif
#define AkieGUI_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

/* 内存块头 */
typedef struct AkieGUI_Mem_Block {
    struct AkieGUI_Mem_Block *next;
    uint32_t size;
    uint32_t magic;
    uint8_t used;
} AkieGUI_Mem_Block_T;

/* 内存管理器 */
typedef struct {
    uint8_t *pool_start;
    uint32_t pool_size;
    uint32_t free_size;
    uint32_t block_count;
    AkieGUI_Mem_Block_T *free_list;
} AkieGUI_Mem_T;

/* 统一内存管理API */
int AkieGUI_MemInit(void *start, uint32_t size);
void* AkieGUI_MemAlloc(uint32_t size);
void* AkieGUI_MemAllocAlign(uint32_t size, uint32_t align);
void* AkieGUI_MemCalloc(uint32_t nmemb, uint32_t size);
void AkieGUI_MemFree(void *ptr);
uint32_t AkieGUI_MemGetFree(void);
uint32_t AkieGUI_MemGetUsed(void);

#endif
