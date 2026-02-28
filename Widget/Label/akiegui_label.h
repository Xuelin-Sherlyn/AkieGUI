/* ============= akiegui_label.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 标签部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_LABEL_H__
#define __AKIEGUI_LABEL_H__

#include "akiegui_widget.h"
#include "akiegui_font_ascii.h"

/* 创建标签 */
AkieGUI_Widget_T* AkieGUI_Label_Create(
    uint16_t x, uint16_t y,
    const char *text,
    uint32_t text_color,    /* RGB888 例如 0xFFFFFF */
    uint32_t bg_color,      /* RGB888 例如 0x000000，0xFFFF00=透明 */
    pFONT *font
);

/* 设置标签文字 */
void AkieGUI_Label_SetText(AkieGUI_Widget_T *label, const char *text);

/* 设置标签颜色 */
void AkieGUI_Label_SetColor(AkieGUI_Widget_T *label, uint32_t text_color);

/* 设置标签背景色 */
void AkieGUI_Label_SetBgColor(AkieGUI_Widget_T *label, uint32_t bg_color);

#endif