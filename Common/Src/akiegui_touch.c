/* ============= akiegui_touch.c ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 触摸驱动部分
 *
 * 许可证: Apache 2.0
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#include "akiegui_touch.h"
/*======== 把触摸取驱动的头文件在这引用 ========*/

/*==========================================*/

static uint16_t last_x = 0, last_y = 0;
static uint8_t last_pressed = 0;

void akiegui_touch_read(uint16_t *x, uint16_t *y, uint8_t *pressed) {
    *x = last_x;
    *y = last_y;
    *pressed = last_pressed;

    // 如果你的触摸驱动有「边沿检测」，可以在这里更新
    // 比如：
    // last_pressed = HAL_GPIO_ReadPin(TOUCH_INT_PIN);
    // 获取坐标...
}