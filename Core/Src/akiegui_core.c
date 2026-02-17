/* ============= akiegui_core.c ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#include "akiegui_core.h"
#include "akiegui_config.h"
#include "akiegui_memory.h"
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

int AkieGUI_FBInit() {
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