/* ============= akiegui_config.h ============= */
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
#ifndef __AKIEGUI_CONFIG_H__
#define __AKIEGUI_CONFIG_H__

#include <stdint.h>

/* ============= 显示屏参数配置 ============= */
#define AkieGUI_LCD_WIDTH   320        /* 显示屏宽度方向像素 */
#define AkieGUI_LCD_HEIGHT  240        /* 显示屏高度方向像素 */
#define AkieGUI_LCD_BPP     32         /* 显示屏像素位深 */

/* ============= FreeRTOS适配配置 ============= */
#define AkieGUI_USE_FREERTOS 0         /* 0=裸机内存池, 1=FreeRTOS堆 */

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
