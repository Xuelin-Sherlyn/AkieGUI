/* ============= akiegui_draw.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 图形绘制部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_DRAW_H__
#define __AKIEGUI_DRAW_H__

#include "akiegui_core.h"
#include "akiegui_color.h"
#include "akiegui_font_ascii.h"

/* 绘制矩形 */
void akiegui_draw_rect(
    void *fb,
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    akiegui_color_t color
);

/* 绘制字符 */
void akiegui_draw_char(
    void *fb,
    uint16_t x, uint16_t y,
    char ch,
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *font
);

/* 绘制字符串 */
void akiegui_draw_string(
    void *fb,
    uint16_t x, uint16_t y,
    const char *str,
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *font
);

/* 计算字符串宽度 */
uint16_t akiegui_text_width(const char *str, pFONT *font);

#endif