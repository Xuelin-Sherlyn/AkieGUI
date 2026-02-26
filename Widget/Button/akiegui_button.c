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
#include "akiegui_font_ascii.h"
#include <string.h>

#define MAX_BUTTONS 10  /* 最大按钮数量 */

/* 按钮私有数据 - 静态数组 */
static struct {
    AkieGUI_Widget_T btn;
    Button_Private priv;
} g_buttons[MAX_BUTTONS];

static uint8_t g_button_count = 0;

/* RGB888 转 RGB565 */
static inline uint16_t rgb888_to_rgb565(uint32_t rgb) {
    uint16_t r = (rgb >> 19) & 0x1F;       /* 取高5位 */
    uint16_t g = (rgb >> 10) & 0x3F;       /* 取中间6位 */
    uint16_t b = (rgb >> 3) & 0x1F;        /* 取低5位 */
    return (r << 11) | (g << 5) | b;
}

/**
  * @brief	矩形绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 矩形坐标 X
  *	@param	y: 矩形坐标 Y
  *	@param	w: 矩形宽度
  *	@param	h: 矩形高度
  *	@param	color: 矩形颜色
*/
static inline void draw_rect(uint16_t *fb, uint16_t x, uint16_t y, 
                            uint16_t w, uint16_t h, uint16_t color) {
    uint16_t fb_width = g_akiegui.fb_width;
    for (uint16_t row = 0; row < h; row++) {
        uint32_t base = (y + row) * fb_width + x;
        for (uint16_t col = 0; col < w; col++) {
            fb[base + col] = color;
        }
    }
}

/**
  * @brief	字符绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 字符坐标 X
  *	@param	y: 字符坐标 Y
  * @param  ch: 字符
  *	@param	color: 字符颜色
  *	@param	bg_color: 背景颜色
  * @param  font: 使用的字体
*/
static void draw_char(uint16_t *fb, uint16_t x, uint16_t y, char ch,
                     uint16_t color, uint16_t bg_color, pFONT *font) {
    if (!font || !font->pTable || ch < 32 || ch > 126) return;
    
    uint16_t fb_width = g_akiegui.fb_width;
    uint16_t offset = (ch - 32) * font->Sizes;
    uint8_t bytes_per_row = (font->Width + 7) / 8;
    
    for (uint8_t row = 0; row < font->Height; row++) {
        for (uint8_t byte = 0; byte < bytes_per_row; byte++) {
            uint8_t data = font->pTable[offset + row * bytes_per_row + byte];
            uint8_t start_col = byte * 8;
            
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t col = start_col + bit;
                if (col >= font->Width) break;
                
                uint32_t fb_idx = (y + row) * fb_width + (x + col);
                fb[fb_idx] = (data & (1 << (7 - bit))) ? color : bg_color;
            }
        }
    }
}

/**
  * @brief	按钮绘制
  *	@param	btn: 按钮句柄
  *	@param	fb: 绘制缓冲区
*/
static void button_draw(AkieGUI_Widget_T *btn, void *fb) {
    Button_Private *priv = (Button_Private*)btn->priv;
    uint16_t *fb16 = (uint16_t*)fb;
    
    uint16_t bg = priv->bg_color;
    if (btn->state & AKIEGUI_STATE_PRESSED) {
        bg = priv->press_color;
    }
    
    draw_rect(fb16, btn->x, btn->y, btn->w, btn->h, bg);
    
    if (btn->border_width > 0) {
        draw_rect(fb16, btn->x, btn->y, btn->w, btn->border_width, btn->border_color);
        draw_rect(fb16, btn->x, btn->y + btn->h - btn->border_width, 
                 btn->w, btn->border_width, btn->border_color);
        draw_rect(fb16, btn->x, btn->y, btn->border_width, btn->h, btn->border_color);
        draw_rect(fb16, btn->x + btn->w - btn->border_width, btn->y, 
                 btn->border_width, btn->h, btn->border_color);
    }
    
    if (priv->text[0] != '\0' && priv->font) {
        pFONT *font = priv->font;
        uint16_t text_len = strlen(priv->text);
        uint16_t text_w = text_len * font->Width;
        uint16_t text_h = font->Height;
        uint16_t start_x = btn->x + (btn->w - text_w) / 2;
        uint16_t start_y = btn->y + (btn->h - text_h) / 2;
        
        for (uint16_t i = 0; i < text_len; i++) {
            draw_char(fb16, start_x + i * font->Width, start_y,
                     priv->text[i], priv->text_color, bg, font);
        }
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
    priv->text_color = rgb888_to_rgb565(text_color);
    priv->bg_color = rgb888_to_rgb565(bg_color);
    priv->press_color = rgb888_to_rgb565(press_color);
    priv->font = &ASCII_8x16;
    
    btn->type = AKIEGUI_WIDGET_BUTTON;
    btn->x = x;
    btn->y = y;
    btn->w = w;
    btn->h = h;
    btn->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    btn->border_color = rgb888_to_rgb565(0x000000);
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

/* 设置按钮文本 */
void AkieGUI_Button_SetText(AkieGUI_Widget_T *btn, const char *text) {
    if (!btn || btn->type != AKIEGUI_WIDGET_BUTTON || !text) return;
    
    Button_Private *priv = (Button_Private*)btn->priv;
    strncpy(priv->text, text, sizeof(priv->text) - 1);
    priv->text[sizeof(priv->text) - 1] = '\0';  /* 确保结尾有\0 */
    btn->dirty = 1;  /* 标记需要重绘 */
}

/* 单独设置颜色 */
void AkieGUI_Button_SetColors(
    AkieGUI_Widget_T *btn,
    uint32_t text_color,
    uint32_t bg_color,
    uint32_t press_color
) {
    if (!btn || btn->type != AKIEGUI_WIDGET_BUTTON) return;
    
    Button_Private *priv = (Button_Private*)btn->priv;
    priv->text_color = rgb888_to_rgb565(text_color);
    priv->bg_color = rgb888_to_rgb565(bg_color);
    priv->press_color = rgb888_to_rgb565(press_color);
    btn->dirty = 1;
}