/* ============= akiegui_memory.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_MEMORY_H__
#define __AKIEGUI_MEMORY_H__

#include "akiegui_config.h"
#if AkieGUI_USE_FREERTOS
#include "FreeRTOS.h"
#endif

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

/* FreeRTOS 适配开关 */
#ifndef AkieGUI_USE_FREERTOS
#define AkieGUI_USE_FREERTOS 0
#endif

/* ===== 临界区保护 === == */
#if AkieGUI_USE_FREERTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #define AkieGUI_ENTER_CRITICAL() taskENTER_CRITICAL()
    #define AkieGUI_EXIT_CRITICAL()  taskEXIT_CRITICAL()
#else
    /* 裸机：直接关中断（ARM Cortex-M）*/
    #define AkieGUI_ENTER_CRITICAL() do { \
        __disable_irq(); \
    } while(0)
    
    #define AkieGUI_EXIT_CRITICAL() do { \
        __enable_irq(); \
    } while(0)
#endif

/* 统一内存管理API */
int AkieGUI_MemInit(void *start, uint32_t size);
void* AkieGUI_MemAlloc(uint32_t size);
void* AkieGUI_MemAllocAlign(uint32_t size, uint32_t align);
void* AkieGUI_MemCalloc(uint32_t nmemb, uint32_t size);
void AkieGUI_MemFree(void *ptr);
uint32_t AkieGUI_MemGetFree(void);
uint32_t AkieGUI_MemGetUsed(void);

#endif
