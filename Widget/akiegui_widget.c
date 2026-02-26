/* ============= akiegui_widget.c ============= */
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
#include "akiegui_widget.h"
#include <string.h>

#define MAX_WIDGETS 20

static struct {
    AkieGUI_Widget_T *widgets[MAX_WIDGETS];
    uint8_t count;
} g_widget_list;

/**
  * @brief	Widget初始化
*/
void AkieGUI_Widget_Init(void) {
    memset(&g_widget_list, 0, sizeof(g_widget_list));
}

/**
  * @brief	控件添加
  *	@param	widget: widget句柄
*/
void AkieGUI_Widget_Add(AkieGUI_Widget_T *widget) {
    if (!widget) return;
    if (g_widget_list.count >= MAX_WIDGETS) return;
    g_widget_list.widgets[g_widget_list.count++] = widget;
}

/**
  * @brief	标记脏矩形
  *	@param	widget: widget句柄
*/
void AkieGUI_Widget_MarkDirty(AkieGUI_Widget_T *widget) {
    if (widget) widget->dirty = 1;
}

/**
  * @brief	绘制所有脏控件，并提交
*/
void AkieGUI_Widget_DrawDirtyAll(void) {
    void *fb = AkieGUI_GetDrawFB();
    uint8_t has_dirty = 0;
    
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *w = g_widget_list.widgets[i];
        if (w && (w->state & AKIEGUI_STATE_VISIBLE) && w->dirty && w->draw) {
            w->draw(w, fb);        /* 绘制到显存 */
            has_dirty = 1;
        }
    }
    
    if (has_dirty) {
        AkieGUI_Commit();          /* 提交到屏幕 */
    }
}

/**
  * @brief	强制重绘所有控件 (全屏刷新)
*/
void AkieGUI_Widget_RedrawAll(void) {
    void *fb = AkieGUI_GetDrawFB();
    
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *w = g_widget_list.widgets[i];
        if (w && (w->state & AKIEGUI_STATE_VISIBLE) && w->draw) {
            w->draw(w, fb);
        }
    }
    
    AkieGUI_Commit();
}

/**
  * @brief	控件点击测试
  * @param  x: 模拟点击的X坐标
  * @param  y: 模拟点击的Y坐标
*/
AkieGUI_Widget_T* AkieGUI_Widget_HitTest(uint16_t x, uint16_t y) {
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *w = g_widget_list.widgets[i];
        if (!w || !(w->state & AKIEGUI_STATE_VISIBLE)) continue;
        
        if (x >= w->x && x < w->x + w->w &&
            y >= w->y && y < w->y + w->h) {
            return w;
        }
    }
    return NULL;
}