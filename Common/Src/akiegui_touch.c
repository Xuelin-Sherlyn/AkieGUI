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