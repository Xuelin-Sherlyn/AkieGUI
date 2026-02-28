/* ============= akiegui_button.c ============= */
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
#include "akiegui_button.h"
#include "akiegui_color.h"
#include "akiegui_draw.h"
#include "akiegui_font_ascii.h"
#include <string.h>

#define MAX_BUTTONS 10  /* 最大按钮数量 */

/* 按钮私有数据 - 静态数组 */
static struct {
    AkieGUI_Widget_T btn;
    Button_Private priv;
} g_buttons[MAX_BUTTONS];

static uint8_t g_button_count = 0;

/**
  * @brief	按钮绘制
  *	@param	btn: 按钮句柄
  *	@param	fb: 绘制缓冲区
*/
static void button_draw(AkieGUI_Widget_T *btn, void *fb) {
    Button_Private *priv = (Button_Private*)btn->priv;
    
    /* 选择背景色 */
    akiegui_color_t bg = priv->bg_color;
    if (btn->state & AKIEGUI_STATE_PRESSED) {
        bg = priv->press_color;
    }
    
    /* 绘制背景 */
    akiegui_draw_rect(fb, btn->x, btn->y, btn->w, btn->h, bg);
    
    /* 绘制边框 */
    if (btn->border_width > 0) {
        akiegui_draw_rect(fb, btn->x, btn->y, btn->w, btn->border_width, btn->border_color);
        akiegui_draw_rect(fb, btn->x, btn->y + btn->h - btn->border_width, 
                         btn->w, btn->border_width, btn->border_color);
        akiegui_draw_rect(fb, btn->x, btn->y, btn->border_width, btn->h, btn->border_color);
        akiegui_draw_rect(fb, btn->x + btn->w - btn->border_width, btn->y, 
                         btn->border_width, btn->h, btn->border_color);
    }
    
    /* 绘制文字 - 居中 */
    if (priv->text[0] != '\0' && priv->font) {
        uint16_t text_w = akiegui_text_width(priv->text, priv->font);
        uint16_t text_h = priv->font->Height;
        uint16_t start_x = btn->x + (btn->w - text_w) / 2;
        uint16_t start_y = btn->y + (btn->h - text_h) / 2;
        
        akiegui_draw_string(fb, start_x, start_y, priv->text,
                           priv->text_color, bg, 0, priv->font);
    }
    
    btn->dirty = 0;
}

/**
  * @brief	按钮创建
  *	@param	x: 按钮坐标 X
  *	@param	y: 按钮坐标 Y
  * @param  w: 按钮宽度
  * @param  h: 按钮高度
  *	@param	text: 按钮的文本
  *	@param	text_color: 按钮文本颜色
  *	@param	bg_color: 按钮的颜色
  *	@param	press_color: 按钮按下后的颜色
  * @retval	AkieGUI_Widget_T实例
*/
AkieGUI_Widget_T* AkieGUI_Button_Create(
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    const char *text,
    uint32_t text_color,
    uint32_t bg_color,
    uint32_t press_color
) {
    if (g_button_count >= MAX_BUTTONS) return NULL;
    
    AkieGUI_Widget_T *btn = &g_buttons[g_button_count].btn;
    Button_Private *priv = &g_buttons[g_button_count].priv;
    
    memset(btn, 0, sizeof(AkieGUI_Widget_T));
    memset(priv, 0, sizeof(Button_Private));
    
    if (text) strncpy(priv->text, text, sizeof(priv->text) - 1);
    
    /* 转换RGB888 → RGB565 */
    priv->text_color = akiegui_rgb888_to_native(text_color);
    priv->bg_color = akiegui_rgb888_to_native(bg_color);
    priv->press_color = akiegui_rgb888_to_native(press_color);
    priv->font = &ASCII_8x16;
    
    btn->type = AKIEGUI_WIDGET_BUTTON;
    btn->x = x;
    btn->y = y;
    btn->w = w;
    btn->h = h;
    btn->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    btn->border_color = akiegui_rgb888_to_native(0x000000);
    btn->border_width = 1;
    btn->dirty = 1;
    btn->draw = button_draw;
    btn->priv = priv;
    
    g_button_count++;
    return btn;
}

/**
  * @brief	设置按钮字体
  *	@param	btn: 按钮句柄
  *	@param	font: 要使用的字体
*/
void AkieGUI_Button_SetFont(AkieGUI_Widget_T *btn, pFONT *font) {
    if (!btn || btn->type != AKIEGUI_WIDGET_BUTTON || !font) return;
    
    Button_Private *priv = (Button_Private*)btn->priv;
    priv->font = font;
    btn->dirty = 1;  /* 标记需要重绘 */
}

/**
  * @brief	设置按钮文本
  *	@param	btn: 按钮句柄
  *	@param	text: 目标按钮要修改到的文本
*/
void AkieGUI_Button_SetText(AkieGUI_Widget_T *btn, const char *text) {
    if (!btn || btn->type != AKIEGUI_WIDGET_BUTTON || !text) return;
    
    Button_Private *priv = (Button_Private*)btn->priv;
    strncpy(priv->text, text, sizeof(priv->text) - 1);
    priv->text[sizeof(priv->text) - 1] = '\0';  /* 确保结尾有\0 */
    btn->dirty = 1;  /* 标记需要重绘 */
}

/**
  * @brief	设置按钮颜色
  *	@param	btn: 按钮句柄
  *	@param	text_color: 按钮文本颜色
  *	@param	bg_color: 按钮的颜色
  *	@param	press_color: 按钮按下后的颜色
*/
void AkieGUI_Button_SetColors(
    AkieGUI_Widget_T *btn,
    uint32_t text_color,
    uint32_t bg_color,
    uint32_t press_color
) {
    if (!btn || btn->type != AKIEGUI_WIDGET_BUTTON) return;
    
    Button_Private *priv = (Button_Private*)btn->priv;
    priv->text_color = akiegui_rgb888_to_native(text_color);
    priv->bg_color = akiegui_rgb888_to_native(bg_color);
    priv->press_color = akiegui_rgb888_to_native(press_color);
    btn->dirty = 1;
}