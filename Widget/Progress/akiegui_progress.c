/* ============= akiegui_progress.c ============= */
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
#include "akiegui_progress.h"
#include "akiegui_color.h"
#include "akiegui_draw.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_PROGRESS 4

static struct {
    AkieGUI_Widget_T widget;
    Progress_Private priv;
} g_progresses[MAX_PROGRESS];

static uint8_t g_progress_count = 0;

static void progress_draw(AkieGUI_Widget_T *widget, void *fb) {
    Progress_Private *priv = (Progress_Private*)widget->priv;

    /* 背景 */
    akiegui_draw_rect(fb, widget->x, widget->y, widget->w, widget->h, priv->bg_color);

    /* 进度条宽度（像素整数） */
    uint16_t fill_w = 0;
    if (priv->max > 0) {
        fill_w = (uint16_t)((uint32_t)widget->w * priv->value / priv->max);
    }
    if (fill_w > 0) {
        akiegui_draw_rect(fb, widget->x, widget->y, fill_w, widget->h, priv->bar_color);
    }

    /* 边框 */
    if (priv->border_width > 0) {
        akiegui_draw_rect(fb, widget->x, widget->y, widget->w, priv->border_width, priv->border_color);
        akiegui_draw_rect(fb, widget->x, widget->y + widget->h - priv->border_width,
                          widget->w, priv->border_width, priv->border_color);
        akiegui_draw_rect(fb, widget->x, widget->y, priv->border_width, widget->h, priv->border_color);
        akiegui_draw_rect(fb, widget->x + widget->w - priv->border_width, widget->y,
                          priv->border_width, widget->h, priv->border_color);
    }

    /* 百分比文字 */
    if (priv->show_percent && priv->font) {
        char buf[8];
        uint16_t percent = (uint16_t)((uint32_t)priv->value * 100 / priv->max);
        snprintf(buf, sizeof(buf), "%d%%", percent);
        uint16_t tw = akiegui_text_width(buf, priv->font);
        uint16_t th = priv->font->Height;
        uint16_t tx = widget->x + (widget->w - tw) / 2;
        uint16_t ty = widget->y + (widget->h - th) / 2;
        akiegui_draw_string(fb, tx, ty, buf,
                            priv->text_color,
                            priv->bg_color,
                            1,
                            priv->font);
    }

    widget->dirty = 0;
}

AkieGUI_Widget_T* AkieGUI_Progress_Create(
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    uint16_t max,
    uint32_t bg_color,
    uint32_t bar_color
) {
    if (g_progress_count >= MAX_PROGRESS) return NULL;

    AkieGUI_Widget_T *widget = &g_progresses[g_progress_count].widget;
    Progress_Private *priv = &g_progresses[g_progress_count].priv;

    memset(widget, 0, sizeof(AkieGUI_Widget_T));
    memset(priv, 0, sizeof(Progress_Private));

    widget->x = x;
    widget->y = y;
    widget->w = w;
    widget->h = h;
    widget->type = AKIEGUI_WIDGET_PROGRESS;
    widget->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    widget->dirty = 1;
    widget->draw = progress_draw;
    widget->priv = priv;

    priv->value = 0;
    priv->max = (max == 0) ? 1 : max;
    priv->bg_color = akiegui_argb888_to_native(bg_color);
    priv->bar_color = akiegui_argb888_to_native(bar_color);
    priv->border_color = akiegui_argb888_to_native(0xFF000000);
    priv->border_width = 1;
    priv->show_percent = 0;

    g_progress_count++;
    return widget;
}

void AkieGUI_Progress_SetValue(AkieGUI_Widget_T *widget, uint16_t value) {
    if (!widget || widget->type != AKIEGUI_WIDGET_PROGRESS) return;
    Progress_Private *priv = (Progress_Private*)widget->priv;
    if (value > priv->max) value = priv->max;
    if (priv->value == value) return;
    priv->value = value;
    widget->dirty = 1;
}

void AkieGUI_Progress_SetMax(AkieGUI_Widget_T *widget, uint16_t max) {
    if (!widget || widget->type != AKIEGUI_WIDGET_PROGRESS) return;
    Progress_Private *priv = (Progress_Private*)widget->priv;
    if (max == 0) return;
    priv->max = max;
    if (priv->value > max) priv->value = max;
    widget->dirty = 1;
}

void AkieGUI_Progress_SetColor(AkieGUI_Widget_T *widget, uint32_t bar_color) {
    if (!widget || widget->type != AKIEGUI_WIDGET_PROGRESS) return;
    Progress_Private *priv = (Progress_Private*)widget->priv;
    if (priv->bar_color == bar_color) return;
    priv->bar_color = bar_color;
    widget->dirty = 1;
}

void AkieGUI_Progress_ShowPercent(AkieGUI_Widget_T *widget,
                                  uint8_t enable,
                                  pFONT *font,
                                  uint32_t text_color) {
    if (!widget || widget->type != AKIEGUI_WIDGET_PROGRESS) return;
    Progress_Private *priv = (Progress_Private*)widget->priv;
    priv->show_percent = enable;
    priv->font = font;
    priv->text_color = akiegui_argb888_to_native(text_color);
    widget->dirty = 1;
}