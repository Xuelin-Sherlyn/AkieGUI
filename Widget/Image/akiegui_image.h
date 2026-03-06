/* ============= akiegui_image.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 图像控件
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_IMAGE_H__
#define __AKIEGUI_IMAGE_H__

#include "akiegui_widget.h"
#include "akiegui_color.h"

/* 图片信息结构体 - 数据格式固定为 RGB888 */
typedef struct {
    uint16_t width;
    uint16_t height;
    const void* data;        /* RGB888 数据指针 (R,G,B 顺序) */
    uint32_t data_size;         /* 数据大小（字节） */
} AkieGUI_Image_Info_T;

/* 创建图片控件 */
AkieGUI_Widget_T* AkieGUI_Image_Create(
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    AkieGUI_Image_Info_T *img_info
);

/* 更新图片内容 */
void AkieGUI_Image_SetData(
    AkieGUI_Widget_T *img,
    AkieGUI_Image_Info_T *img_info
);

#endif