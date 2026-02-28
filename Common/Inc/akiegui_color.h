/* ============= akiegui_color.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 颜色转换部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_COLOR_H__
#define __AKIEGUI_COLOR_H__

#include "akiegui_config.h"
#include <stdint.h>

/* ===== 根据 BPP 选择颜色转换 ===== */

#if AkieGUI_LCD_BPP == 16
/* RGB888 转 RGB565 */
static inline uint16_t akiegui_rgb888_to_native(uint32_t rgb) {
    uint16_t r = (rgb >> 19) & 0x1F;
    uint16_t g = (rgb >> 10) & 0x3F;
    uint16_t b = (rgb >> 3) & 0x1F;
    return (r << 11) | (g << 5) | b;
}

/* 本地颜色转 RGB888 */
static inline uint32_t akiegui_native_to_rgb888(uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
    
    return (r << 16) | (g << 8) | b;
}

/* 本地颜色类型 */
typedef uint16_t akiegui_color_t;

#elif AkieGUI_LCD_BPP == 24
/* RGB888 直接就是本地格式 */
static inline uint32_t akiegui_rgb888_to_native(uint32_t rgb) {
    return rgb & 0x00FFFFFF;
}

static inline uint32_t akiegui_native_to_rgb888(uint32_t color) {
    return color & 0x00FFFFFF;
}

typedef uint32_t akiegui_color_t;

#elif AkieGUI_LCD_BPP == 32
/* ARGB8888 格式（A 固定为 0xFF）*/
static inline uint32_t akiegui_rgb888_to_native(uint32_t rgb) {
    return 0xFF000000 | (rgb & 0x00FFFFFF);
}

static inline uint32_t akiegui_native_to_rgb888(uint32_t color) {
    return color & 0x00FFFFFF;
}

typedef uint32_t akiegui_color_t;

#else
#error "AkieGUI_LCD_BPP must be 16, 24, or 32"
#endif

/* 预定义颜色（RGB888 格式）*/
#define AKIEGUI_RED     0xFF0000
#define AKIEGUI_GREEN   0x00FF00
#define AKIEGUI_BLUE    0x0000FF
#define AKIEGUI_WHITE   0xFFFFFF
#define AKIEGUI_BLACK   0x000000
#define AKIEGUI_YELLOW  0xFFFF00
#define AKIEGUI_CYAN    0x00FFFF
#define AKIEGUI_MAGENTA 0xFF00FF
#define AKIEGUI_GRAY    0x808080
#define AKIEGUI_LTGRAY  0xC0C0C0
#define AKIEGUI_DKGRAY  0x404040

#endif