/* ============= akiegui_config.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 用户配置文件
 * 
 * 在这里配置：
 *   - 屏幕参数（宽/高/色深）
 *   - 功能开关（FreeRTOS/TE/双缓冲）
 *   - 硬件布局（内部RAM/外部SDRAM）
 * 
 * 所有配置项都提供默认值，用户可自行覆盖
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_CONFIG_H__
#define __AKIEGUI_CONFIG_H__

#include <stdint.h>

/* ============= 显示屏参数配置 ============= */
#define AkieGUI_LCD_WIDTH   320        /* 显示屏宽度方向像素 */
#define AkieGUI_LCD_HEIGHT  240        /* 显示屏高度方向像素 */
#define AkieGUI_LCD_BPP     16         /* 显示屏像素位深 */

/* ============= FreeRTOS适配配置 ============= */
#define AkieGUI_USE_FREERTOS 0         /* 0=裸机内存池, 1=FreeRTOS堆 */

/* FreeRTOS 默认配置 */
#ifndef AkieGUI_USE_FREERTOS
#define AkieGUI_USE_FREERTOS 0
#endif

/* ============= TE模式配置 ============= */
#define AkieGUI_KE_NO_TE     0        /* 不等待TE，发送完即继续 */
#define AkieGUI_KE_TE_EN     1        /* 等待TE，任何传输完成通知 */

/* 默认无TE模式，用户可在编译选项覆盖 */
#ifndef AkieGUI_TE_MODE
#define AkieGUI_TE_MODE     AkieGUI_KE_NO_TE        /* 0=不等待, 1=等待 */
#endif

/* ============= 双缓冲模式配置 ============= */
#define AkieGUI_NO_DOUBLE_BUFFER    0 /* 使用单缓冲模式 */
#define AkieGUI_DOUBLE_BUFFER_EN    1 /* 使用双缓冲模式 */

/* 默认单缓冲模式，用户可在编译选项覆盖 */
#ifndef AkieGUI_DOUBLE_BUFFER_MODE
#define AkieGUI_DOUBLE_BUFFER_MODE    AkieGUI_NO_DOUBLE_BUFFER
#endif

/* ============= 显存位置配置 ============= */
#define AkieGUI_MEM_INTERNAL    0    /* 内部RAM */
#define AkieGUI_MEM_EXTERNAL    1    /* 外部SDRAM */

#ifndef AkieGUI_MEM_TYPE
#define AkieGUI_MEM_TYPE    AkieGUI_MEM_INTERNAL    /* 0=内部RAM, 1=外部SDRAM */
#endif

#if AkieGUI_MEM_TYPE == AkieGUI_MEM_EXTERNAL
#define AkieGUI_MEM_ADDR 0xC0000000
#endif

#endif
