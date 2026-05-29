/* ============= akiegui_core.c ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 图形核心实现
 * 
 * 实现帧缓冲的初始化：
 *   - 计算缓冲区大小
 *   - 分配内存（通过memory层）
 *   - 设置双缓冲/单缓冲模式
 * 
 * 其他操作都在头文件中内联实现
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#include "akiegui.h"
#include "akiegui_core.h"
#include "akiegui_config.h"
#include "akiegui_memory.h"
#include "akiegui_touch.h"
#include "stdint.h"
#include "stddef.h"

/* 全局实例定义 */
AkieGUI_t g_akiegui = {
#if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    .te_pending = 0,
#endif
    .send_frame = NULL,
    .send_region = NULL,
    .fb1 = NULL,
    .fb2 = NULL,
    .draw_fb = NULL,
    .disp_fb = NULL,
    .fb_width = 0,
    .fb_height = 0,
    .fb_bpp = 0,
    .fb_size = 0,
    .double_buffer = 0,
    .screen_width = 0,
    .screen_height = 0,
    .screen_bpp = 0,
    .user_data = NULL
};

static AkieGUI_Widget_T *g_touch_down_widget = NULL;

/**
 * @brief 初始化帧缓冲
 * @retval 0成功 -1失败
 */
int AkieGUI_FBInit(void) {
    /* 计算帧缓冲大小 */
    uint32_t bytes_per_pixel = AkieGUI_LCD_BPP / 8;
    if (bytes_per_pixel == 0) bytes_per_pixel = 2;  /* 默认RGB565 */
    
    uint32_t fb_size = AkieGUI_LCD_WIDTH * AkieGUI_LCD_HEIGHT * bytes_per_pixel;
    fb_size = AkieGUI_ALIGN_UP(fb_size, AkieGUI_ALIGN);
    
    /* 分配缓冲区1 */
    g_akiegui.fb1 = (uint8_t*)AkieGUI_MemAllocAlign(fb_size, 32);
    if (!g_akiegui.fb1) return -1;
    
    /* 分配缓冲区2（如果需要）*/
    if (AkieGUI_DOUBLE_BUFFER_MODE) {
        g_akiegui.fb2 = (uint8_t*)AkieGUI_MemAllocAlign(fb_size, 32);
        if (!g_akiegui.fb2) {
            AkieGUI_MemFree(g_akiegui.fb1);
            return -2;
        }
        g_akiegui.draw_fb = g_akiegui.fb1;
        g_akiegui.disp_fb = g_akiegui.fb2;
    } else {
        g_akiegui.fb2 = NULL;
        g_akiegui.draw_fb = g_akiegui.fb1;
        g_akiegui.disp_fb = g_akiegui.fb1;
    }
    
    /* 保存参数 */
    g_akiegui.fb_width = AkieGUI_LCD_WIDTH;
    g_akiegui.fb_height = AkieGUI_LCD_HEIGHT;
    g_akiegui.fb_bpp = AkieGUI_LCD_BPP;
    g_akiegui.fb_size = fb_size;
    g_akiegui.double_buffer = AkieGUI_DOUBLE_BUFFER_MODE;
    g_akiegui.screen_width = AkieGUI_LCD_WIDTH;
    g_akiegui.screen_height = AkieGUI_LCD_HEIGHT;
    g_akiegui.screen_bpp = AkieGUI_LCD_BPP;
    
    return 0;
}

void AkieGUI_ProcessTouch(void) {
    uint16_t x, y;
    uint8_t pressed;
    static uint8_t last_pressed = 0;

    akiegui_touch_read(&x, &y, &pressed);

    if (pressed && !last_pressed) {
        // 按下：记录命中的控件
        g_touch_down_widget = AkieGUI_Widget_HitTest(x, y);
        if (g_touch_down_widget) {
            g_touch_down_widget->state |= AKIEGUI_STATE_PRESSED;
            AkieGUI_Widget_MarkDirty(g_touch_down_widget);
            AkieGUI_Widget_RedrawDirtyRegion();   // 或只刷新脏控件
            if(g_touch_down_widget->on_click)
            {
                g_touch_down_widget->on_click(g_touch_down_widget);
            }
        }
    } else if(!pressed && last_pressed) {
        // 抬起：触发 click（如果按下和抬起是同一个控件）
        if (g_touch_down_widget) {
            if (g_touch_down_widget->on_release) {
                g_touch_down_widget->on_release(g_touch_down_widget);
            }
            g_touch_down_widget->state &= ~AKIEGUI_STATE_PRESSED;
            AkieGUI_Widget_MarkDirty(g_touch_down_widget);
            AkieGUI_Widget_RedrawDirtyRegion();
            g_touch_down_widget = NULL;
        }
    }else {
        return;
    }
    last_pressed = pressed;
}