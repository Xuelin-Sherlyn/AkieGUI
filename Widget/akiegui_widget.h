/* ============= akiegui_widget.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 控件管理器
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_WIDGET_H__
#define __AKIEGUI_WIDGET_H__

#include "akiegui_core.h"

/* 控件类型 */
typedef enum {
    AKIEGUI_WIDGET_NONE = 0,
    AKIEGUI_WIDGET_BUTTON,
    AKIEGUI_WIDGET_LABEL,
} AkieGUI_Widget_Type;

/* 控件状态 */
#define AKIEGUI_STATE_VISIBLE      (1 << 0)
#define AKIEGUI_STATE_ENABLED      (1 << 1)
#define AKIEGUI_STATE_PRESSED      (1 << 2)
#define AKIEGUI_STATE_FOCUSED      (1 << 3)

/* 通用控件结构体 */
typedef struct AkieGUI_Widget {
    AkieGUI_Widget_Type type;
    uint16_t x, y;
    uint16_t w, h;
    uint8_t state;
    
    /* 颜色（RGB565，直接存）*/
    uint16_t forg_color;
    uint16_t back_color;
    uint16_t border_color;
    uint8_t border_width;
    
    /* 字体（指向你的 pFONT）*/
    void *font;
    
    /* 脏标记 - 只在需要重绘时更新 */
    uint8_t dirty;
    
    /* 回调 */
    void (*on_click)(struct AkieGUI_Widget *self);
    void *user_data;
    
    /* 绘制函数 */
    void (*draw)(struct AkieGUI_Widget *self, void *fb);
    
    /* 私有数据 */
    void *priv;
} AkieGUI_Widget_T;

/* 控件管理器 */
void AkieGUI_Widget_Init(void);
void AkieGUI_Widget_Add(AkieGUI_Widget_T *widget);
void AkieGUI_Widget_DrawDirtyAll(void);
void AkieGUI_Widget_RedrawAll(void);
void AkieGUI_Widget_MarkDirty(AkieGUI_Widget_T *widget);
AkieGUI_Widget_T* AkieGUI_Widget_HitTest(uint16_t x, uint16_t y);

#endif