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

/* 全局脏矩形区域（用于局部刷新）*/
static uint16_t dirty_min_x = 0xFFFF;
static uint16_t dirty_min_y = 0xFFFF;
static uint16_t dirty_max_x = 0;
static uint16_t dirty_max_y = 0;

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
  * @brief	从管理器移除控件
  * @param	widget: 要移除的控件指针
  * @retval	无
  */
void AkieGUI_Widget_Remove(AkieGUI_Widget_T *widget) {
    if (!widget) return;
    
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        if (g_widget_list.widgets[i] == widget) {
            /* 把最后一个控件移过来填补空缺 */
            g_widget_list.widgets[i] = g_widget_list.widgets[g_widget_list.count - 1];
            g_widget_list.count--;
            
            /* 标记控件不可见（但不释放内存）*/
            widget->state &= ~AKIEGUI_STATE_VISIBLE;
            widget->dirty = 0;
            return;
        }
    }
}

/**
  * @brief	从管理器移除所有控件
  * @param	无
  * @retval	无
  */
void AkieGUI_Widget_RemoveAll(void) {
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *widget = g_widget_list.widgets[i];
        if (widget) {
            widget->state &= ~AKIEGUI_STATE_VISIBLE;
            widget->dirty = 0;
        }
    }
    g_widget_list.count = 0;
}

/**
  * @brief	标记控件脏，并扩展全局脏矩形
  * @param	widget: 控件句柄
  */
void AkieGUI_Widget_MarkDirty(AkieGUI_Widget_T *widget) {
    if (!widget) return;

    // 1. 原有控件脏标记
    widget->dirty = 1;

    // 2. 扩展全局脏矩形（包含控件全部区域）
    uint16_t right = widget->x + widget->w;
    uint16_t bottom = widget->y + widget->h;

    if (widget->x < dirty_min_x) dirty_min_x = widget->x;
    if (widget->y < dirty_min_y) dirty_min_y = widget->y;
    if (right   > dirty_max_x)   dirty_max_x = right;
    if (bottom  > dirty_max_y)   dirty_max_y = bottom;
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
  * @brief	只重绘全局脏矩形区域
  */
void AkieGUI_Widget_RedrawDirtyRegion(void) {
    if (dirty_min_x > dirty_max_x) return;  // 无脏区域

    // 只绘制与脏矩形相交的控件
    for (int i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *w = g_widget_list.widgets[i];
        if (!(w->state & AKIEGUI_STATE_VISIBLE)) continue;

        // 简单相交判断
        if (w->x + w->w <= dirty_min_x) continue;
        if (w->y + w->h <= dirty_min_y) continue;
        if (w->x >= dirty_max_x) continue;
        if (w->y >= dirty_max_y) continue;

        if (w->dirty && w->draw) {
            w->draw(w, AkieGUI_GetDrawFB());
        }
    }

    // 提交脏矩形到屏幕（节省传输带宽）
    AkieGUI_CommitRegion(dirty_min_x, dirty_min_y,
                         dirty_max_x - dirty_min_x,
                         dirty_max_y - dirty_min_y);

    // 重置脏矩形
    dirty_min_x = 0xFFFF; dirty_min_y = 0xFFFF;
    dirty_max_x = 0;      dirty_max_y = 0;
}

/**
  * @brief	控件点击测试
  * @param  x: 模拟点击的X坐标
  * @param  y: 模拟点击的Y坐标
*/
AkieGUI_Widget_T* AkieGUI_Widget_HitTest(uint16_t x, uint16_t y) {
    for (uint8_t i = 0; i < g_widget_list.count; i++) {
        AkieGUI_Widget_T *w = g_widget_list.widgets[i];
        if (!w || !(w->state & AKIEGUI_STATE_VISIBLE) || !w->on_click) continue;
        
        if (x >= w->x && x < w->x + w->w &&
            y >= w->y && y < w->y + w->h) {
            return w;
        }
    }
    return NULL;
}