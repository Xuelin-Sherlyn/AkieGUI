/* ============= akiegui_port.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 * 移植层
 * 
 * 处理与硬件/操作系统相关的底层操作：
 *   - 架构检测（ARM/RISC-V等）
 *   - 中断控制（PRIMASK操作）
 *   - 临界区保护（裸机/FreeRTOS）
 * 
 * 移植到新平台时，主要修改这个文件
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_PORT_H__
#define __AKIEGUI_PORT_H__

#include "akiegui_config.h"

/* ===== 架构检测与中断控制 ===== */
#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_8M_BASE__) || \
    defined(__ARM_ARCH_8M_MAIN__)
    /* ARM Cortex-M 全系列 */
    #define AkieGUI_ARCH_ARM 1
    #include "stm32h7xx.h"
    
    /* CMSIS 兼容的内置函数 */
    #define AkieGUI_GET_PRIMASK()      __get_PRIMASK()
    #define AkieGUI_SET_PRIMASK(x)     __set_PRIMASK(x)
    #define AkieGUI_DISABLE_IRQ()      __disable_irq()
    #define AkieGUI_ENABLE_IRQ()       __enable_irq()
    
#elif defined(__riscv)
    /* RISC-V (简化版，实际需要根据权限模式调整) */
    #define AkieGUI_ARCH_RISCV 1
    
    static inline uint32_t AkieGUI_GET_MSTATUS(void) {
        uint32_t result;
        asm volatile("csrr %0, mstatus" : "=r"(result));
        return result;
    }
    
    static inline void AkieGUI_SET_MSTATUS(uint32_t val) {
        asm volatile("csrw mstatus, %0" :: "r"(val));
    }
    
    #define AkieGUI_GET_PRIMASK()      AkieGUI_GET_MSTATUS()
    #define AkieGUI_SET_PRIMASK(x)     AkieGUI_SET_MSTATUS(x)
    #define AkieGUI_DISABLE_IRQ()       asm volatile("csrc mstatus, 8")
    #define AkieGUI_ENABLE_IRQ()        asm volatile("csrs mstatus, 8")
    
#else
    /* 未知架构：让用户自己实现 */
    #ifndef AkieGUI_DISABLE_IRQ
    #error "AkieGUI: Please implement AkieGUI_GET_PRIMASK(), AkieGUI_SET_PRIMASK(), AkieGUI_DISABLE_IRQ(), AkieGUI_ENABLE_IRQ() for your platform"
    #endif
#endif

/* ===== 临界区保护 ===== */
#if AkieGUI_USE_FREERTOS
    /* FreeRTOS：使用OS自带的临界区 */
    #include "FreeRTOS.h"
    #include "task.h"
    
    #define AkieGUI_ENTER_CRITICAL(primask_save) \
        do { \
            (void)(primask_save); /* 防止未使用警告 */ \
            taskENTER_CRITICAL(); \
        } while(0)
    
    #define AkieGUI_EXIT_CRITICAL(primask_save) \
        do { \
            (void)(primask_save); \
            taskEXIT_CRITICAL(); \
        } while(0)
    
#else
    /* 裸机：保存恢复PRIMASK（最安全） */
    #define AkieGUI_ENTER_CRITICAL(primask_save) \
        do { \
            (primask_save) = AkieGUI_GET_PRIMASK(); \
            AkieGUI_DISABLE_IRQ(); \
        } while(0)
    
    #define AkieGUI_EXIT_CRITICAL(primask_save) \
        do { \
            AkieGUI_SET_PRIMASK(primask_save); \
        } while(0)
#endif

#endif /* __AKIEGUI_PORT_H__ */