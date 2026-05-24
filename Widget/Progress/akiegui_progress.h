/* ============= akiegui_progress.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 进度条部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_PROGRESS_H__
#define __AKIEGUI_PROGRESS_H__

#include "akiegui_widget.h"
#include "akiegui_font_ascii.h"

typedef struct {
    uint16_t value;
    uint16_t max;
    akiegui_color_t bar_color;
    akiegui_color_t bg_color;
    akiegui_color_t border_color;
    uint8_t border_width;

    uint8_t show_percent;
    pFONT *font;
    uint16_t text_color;
} Progress_Private;

AkieGUI_Widget_T* AkieGUI_Progress_Create(
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    uint16_t max,
    uint32_t bg_color,    /* RGB888 */
    uint32_t bar_color    /* RGB888 */
);

void AkieGUI_Progress_SetValue(AkieGUI_Widget_T *widget, uint16_t value);
void AkieGUI_Progress_SetMax(AkieGUI_Widget_T *widget, uint16_t max);
void AkieGUI_Progress_ShowPercent(AkieGUI_Widget_T *widget,
                                  uint8_t enable,
                                  pFONT *font,
                                  uint32_t text_color);

#endif