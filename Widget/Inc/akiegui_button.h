/* ============= akiegui_button.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 按钮部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_BUTTON_H__
#define __AKIEGUI_BUTTON_H__

#include "akiegui_widget.h"
#include "akiegui_font_ascii.h"
#include <string.h>

/* 按钮数据 */
typedef struct {
    char text[32];
    uint16_t text_color;     /* 文字颜色 */
    uint16_t bg_color;       /* 背景色 */
    uint16_t press_color;    /* 按下颜色 */
    pFONT *font;             /* 字体指针 - 直接用你的pFONT */
} Button_Private;

AkieGUI_Widget_T* AkieGUI_Button_Create(uint16_t x, uint16_t y, 
                                        uint16_t w, uint16_t h, 
                                        const char *text);
void AkieGUI_Button_SetFont(AkieGUI_Widget_T *btn, pFONT *font);
void AkieGUI_Button_SetText(AkieGUI_Widget_T *btn, const char *text);

#endif