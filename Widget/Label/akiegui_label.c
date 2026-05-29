/* ============= akiegui_label.c ============= */
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
#include "akiegui_label.h"
#include "akiegui_draw.h"

#define MAX_LABELS 10

/* 标签私有数据 */
typedef struct {
    char text[64];               /* 文字内容 */
    akiegui_color_t text_color;  /* 文本颜色 */
    akiegui_color_t bg_color;    /* 背景颜色 */
    pFONT* ascii_font;           /* ASCII文本字体 */
    pFONT* chinese_font;         /* 中文文本字体 */
    uint8_t transparent;         /* 1=背景透明，只画文字 */
} Label_Private;

/* 静态标签池 */
static struct {
    AkieGUI_Widget_T label;
    Label_Private priv;
} g_labels[MAX_LABELS];

static uint8_t g_label_count = 0;

/**
  * @brief	标签绘制
  *	@param	lable: 标签句柄
  *	@param	fb: 绘制缓冲区
*/
static void label_draw(AkieGUI_Widget_T *widget, void *fb) {
    Label_Private *priv = (Label_Private*)widget->priv;
    
    /* 如果不透明，先画背景 */
    if (!priv->transparent) {
        akiegui_draw_rect(fb, widget->x, widget->y, widget->w, widget->h, priv->bg_color);
    }
    
    /* 画文字 - 居中 */
    if (priv->text[0] != '\0' && priv->ascii_font) {
        uint16_t text_w = akiegui_text_width(priv->text, priv->ascii_font);
        uint16_t text_h = priv->ascii_font->Height;
        uint16_t start_x = widget->x + (widget->w - text_w) / 2;
        uint16_t start_y = widget->y + (widget->h - text_h) / 2;

        akiegui_draw_string(fb, start_x, start_y, priv->text,
                       priv->text_color, priv->bg_color, 
                       priv->transparent, priv->ascii_font);
    }
    
    widget->dirty = 0;
}

/**
  * @brief  中英文混合标签绘制（GB2312 + ASCII）
  *	@param	lable: 标签句柄
  *	@param	fb: 绘制缓冲区
  */
static void label_draw_mixed(AkieGUI_Widget_T* label, void *fb) {
    Label_Private *priv = (Label_Private*)label->priv;

    /* 如果不透明，先画背景 */
    if (!priv->transparent) {
        akiegui_draw_rect(fb, label->x, label->y, label->w, label->h, priv->bg_color);
    }
    const char* str = priv->text;
    uint16_t tx = label->x + (label->w - strlen(priv->text) * priv->chinese_font->Width) / 2;
    uint16_t ty = label->y + (label->h - priv->chinese_font->Height) / 2;

    akiegui_draw_chinese_string(fb, tx, ty, str, 
        priv->text_color, priv->bg_color, 
        priv->transparent, priv->chinese_font, 
        priv->ascii_font);

    label->dirty = 0;
}

/**
  * @brief	创建标签
  *	@param	x: 标签X坐标
  *	@param	y: 标签Y坐标
  * @param  text: 标签文本
  *	@param	text_color: 标签文本颜色
  *	@param	bg_color: 标签的背景颜色
  *	@param	font: 要使用的字体
  * @retval	AkieGUI_Widget_T实例
*/
AkieGUI_Widget_T* AkieGUI_Label_Create(
    uint16_t x, uint16_t y,
    const char *text,
    uint32_t text_color,
    uint32_t bg_color,
    pFONT *font
) {
    if (g_label_count >= MAX_LABELS) return NULL;
    
    AkieGUI_Widget_T *widget = &g_labels[g_label_count].label;
    Label_Private *priv = &g_labels[g_label_count].priv;
    
    memset(widget, 0, sizeof(AkieGUI_Widget_T));
    memset(priv, 0, sizeof(Label_Private));
    
    if (text) strncpy(priv->text, text, sizeof(priv->text) - 1);
    
    /* 计算标签尺寸（根据文字长度）*/
    // uint16_t text_len = text ? strlen(text) : 0;
    uint16_t width = akiegui_text_width(text, font) + 4;  /* 左右留2像素边 */
    uint16_t height = font->Height + 4;           /* 上下留2像素边 */
    
    /* 颜色转换 */
    priv->text_color = akiegui_argb888_to_native(text_color);
    priv->bg_color = akiegui_argb888_to_native(bg_color);
    priv->ascii_font = font;
#if AkieGUI_LCD_BPP == 16
    priv->transparent = (bg_color == 0xFFFF00);
#else
    priv->transparent = ((bg_color >> 24) == 0);
#endif
    
    widget->type = AKIEGUI_WIDGET_LABEL;
    widget->x = x;
    widget->y = y;
    widget->w = width;
    widget->h = height;
    widget->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    widget->dirty = 1;
    widget->draw = label_draw;
    widget->priv = priv;
    
    g_label_count++;
    return widget;
}

/**
  * @brief	创建中文标签
  *	@param	x: 标签X坐标
  *	@param	y: 标签Y坐标
  * @param  text: 标签文本
  *	@param	text_color: 标签文本颜色
  *	@param	bg_color: 标签的背景颜色
  * @param  ascii_font: 要使用的英文字体
  *	@param	ch_font: 要使用的中文字体
  * @retval	AkieGUI_Widget_T实例
*/
AkieGUI_Widget_T* AkieGUI_Label_Create_Chinese(
    uint16_t x, uint16_t y,
    const char *text,
    uint32_t text_color,
    uint32_t bg_color,
    pFONT *ascii_font,
    pFONT *ch_font
) {
    if (g_label_count >= MAX_LABELS) return NULL;
    
    AkieGUI_Widget_T *widget = &g_labels[g_label_count].label;
    Label_Private *priv = &g_labels[g_label_count].priv;
    
    memset(widget, 0, sizeof(AkieGUI_Widget_T));
    memset(priv, 0, sizeof(Label_Private));
    
    if (text) strncpy(priv->text, text, sizeof(priv->text) - 1);
    
    /* 计算标签尺寸（根据文字长度）*/
    // uint16_t text_len = text ? strlen(text) : 0;
    uint16_t width = akiegui_text_width(text, ch_font) + 4;  /* 左右留2像素边 */
    uint16_t height = ch_font->Height + 4;           /* 上下留2像素边 */
    
    /* 颜色转换 */
    priv->text_color = akiegui_argb888_to_native(text_color);
    priv->bg_color = akiegui_argb888_to_native(bg_color);
    priv->ascii_font = ascii_font;
    priv->chinese_font = ch_font;
    priv->transparent = (bg_color == 0xFFFF00);  /* 0xFFFF00 作为透明色标记 */
    
    widget->type = AKIEGUI_WIDGET_LABEL;
    widget->x = x;
    widget->y = y;
    widget->w = width;
    widget->h = height;
    widget->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    widget->dirty = 1;
    widget->draw = label_draw_mixed;
    widget->priv = priv;
    
    g_label_count++;
    return widget;
}

/**
  * @brief	设置标签文字
  * @param  lable: 标签句柄
  * @param  text: 标签文本
*/
void AkieGUI_Label_SetText(AkieGUI_Widget_T *widget, const char *text) {
    if (!widget || widget->type != AKIEGUI_WIDGET_LABEL || !text) return;
    
    Label_Private *priv = (Label_Private*)widget->priv;
    strncpy(priv->text, text, sizeof(priv->text) - 1);
    priv->text[sizeof(priv->text) - 1] = '\0';
    
    /* 重新计算宽度（如果需要自动调整）*/
    uint16_t text_len = strlen(text);
    widget->w = text_len * priv->ascii_font->Width + 4;
    
    widget->dirty = 1;
}

/**
  * @brief	设置标签文字
  * @param  lable: 标签句柄
  * @param  text: 标签文本
*/
void AkieGUI_Label_SetText_Chinese(AkieGUI_Widget_T *widget, const char *text) {
    if (!widget || widget->type != AKIEGUI_WIDGET_LABEL || !text) return;
    
    Label_Private *priv = (Label_Private*)widget->priv;
    strncpy(priv->text, text, sizeof(priv->text) - 1);
    priv->text[sizeof(priv->text) - 1] = '\0';
    
    /* 重新计算宽度（如果需要自动调整）*/
    uint16_t text_len = strlen(text);
    widget->w = text_len * priv->chinese_font->Width + 4;
    
    widget->dirty = 1;
}

/**
  * @brief	设置标签颜色
  * @param  lable: 标签句柄
  * @param  text_color: 标签文本颜色
*/
void AkieGUI_Label_SetColor(AkieGUI_Widget_T *widget, uint32_t text_color) {
    if (!widget || widget->type != AKIEGUI_WIDGET_LABEL) return;
    
    Label_Private *priv = (Label_Private*)widget->priv;
    priv->text_color = akiegui_argb888_to_native(text_color);
    widget->dirty = 1;
}

/**
  * @brief	设置标签背景颜色
  * @param  lable: 标签句柄
  * @param  bg_color: 标签背景颜色
*/
void AkieGUI_Label_SetBgColor(AkieGUI_Widget_T *widget, uint32_t bg_color) {
    if (!widget || widget->type != AKIEGUI_WIDGET_LABEL) return;
    
    Label_Private *priv = (Label_Private*)widget->priv;
    priv->bg_color = akiegui_argb888_to_native(bg_color);
#if AkieGUI_LCD_BPP == 16
    priv->transparent = (bg_color == 0xFFFF00);
#else
    priv->transparent = ((bg_color >> 24) == 0);
#endif
    widget->dirty = 1;
}