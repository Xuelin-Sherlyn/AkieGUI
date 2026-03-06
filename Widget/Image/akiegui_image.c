/* ============= akiegui_image.c ============= */
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
#include "akiegui_image.h"
#include "akiegui_draw.h"
#include <string.h>

#define MAX_IMAGES 10

/**
  * @brief	图片控件私有数据
  */
typedef struct {
    AkieGUI_Image_Info_T img_info;      /* 原始 RGB888 信息 */
    uint8_t need_scale;                  /* 是否需要缩放（1=是，0=否）*/
} Image_Private;

/* 静态图片池 */
static struct {
    AkieGUI_Widget_T widget;
    Image_Private priv;
} g_images[MAX_IMAGES];

static uint8_t g_image_count = 0;

/**
  * @brief	获取 RGB888 像素并转换为本地颜色格式
  * @param	data: RGB888 数据指针
  * @param	x: 像素 X 坐标
  * @param	y: 像素 Y 坐标
  * @param	width: 图片宽度
  * @retval	转换后的本地颜色值
  */
static inline akiegui_color_t get_pixel_rgb888(const uint8_t *data, uint16_t x, uint16_t y, uint16_t width) {
    uint32_t idx = (y * width + x) * 3;
    uint32_t rgb = (data[idx] << 16) | (data[idx + 1] << 8) | data[idx + 2];
    return akiegui_rgb888_to_native(rgb);
}

/**
  * @brief	绘制图片（不缩放，原尺寸居中）
  * @param	fb: 帧缓冲区指针
  * @param	widget: 控件指针
  * @param	priv: 图片私有数据指针
  * @retval	无
  */
static void draw_image_no_scale(void *fb, AkieGUI_Widget_T *widget, Image_Private *priv) {
    AkieGUI_Image_Info_T *info = &priv->img_info;
    uint16_t fb_width = g_akiegui.fb_width;
    
    uint16_t draw_w = (widget->w < info->width) ? widget->w : info->width;
    uint16_t draw_h = (widget->h < info->height) ? widget->h : info->height;
    
    /* 居中显示 */
    uint16_t start_x = widget->x + (widget->w - draw_w) / 2;
    uint16_t start_y = widget->y + (widget->h - draw_h) / 2;
    
#if AkieGUI_LCD_BPP == 16
    uint16_t *fb16 = (uint16_t*)fb;
    for (uint16_t y = 0; y < draw_h; y++) {
        for (uint16_t x = 0; x < draw_w; x++) {
            akiegui_color_t color = get_pixel_rgb888(info->data, x, y, info->width);
            uint32_t fb_idx = (start_y + y) * fb_width + (start_x + x);
            fb16[fb_idx] = color;
        }
    }
#else
    uint32_t *fb32 = (uint32_t*)fb;
    for (uint16_t y = 0; y < draw_h; y++) {
        for (uint16_t x = 0; x < draw_w; x++) {
            akiegui_color_t color = get_pixel_rgb888(info->data, x, y, info->width);
            uint32_t fb_idx = (start_y + y) * fb_width + (start_x + x);
            fb32[fb_idx] = color;
        }
    }
#endif
}

/**
  * @brief	绘制图片（缩放，最近邻插值）
  * @param	fb: 帧缓冲区指针
  * @param	widget: 控件指针
  * @param	priv: 图片私有数据指针
  * @retval	无
  */
static void draw_image_scaled(void *fb, AkieGUI_Widget_T *widget, Image_Private *priv) {
    AkieGUI_Image_Info_T *info = &priv->img_info;
    uint16_t fb_width = g_akiegui.fb_width;
    
    float scale_x = (float)info->width / widget->w;
    float scale_y = (float)info->height / widget->h;
    
#if AkieGUI_LCD_BPP == 16
    uint16_t *fb16 = (uint16_t*)fb;
    for (uint16_t y = 0; y < widget->h; y++) {
        uint16_t src_y = (uint16_t)(y * scale_y);
        if (src_y >= info->height) src_y = info->height - 1;
        
        for (uint16_t x = 0; x < widget->w; x++) {
            uint16_t src_x = (uint16_t)(x * scale_x);
            if (src_x >= info->width) src_x = info->width - 1;
            
            akiegui_color_t color = get_pixel_rgb888(info->data, src_x, src_y, info->width);
            uint32_t fb_idx = (widget->y + y) * fb_width + (widget->x + x);
            fb16[fb_idx] = color;
        }
    }
#else
    uint32_t *fb32 = (uint32_t*)fb;
    for (uint16_t y = 0; y < widget->h; y++) {
        uint16_t src_y = (uint16_t)(y * scale_y);
        if (src_y >= info->height) src_y = info->height - 1;
        
        for (uint16_t x = 0; x < widget->w; x++) {
            uint16_t src_x = (uint16_t)(x * scale_x);
            if (src_x >= info->width) src_x = info->width - 1;
            
            akiegui_color_t color = get_pixel_rgb888(info->data, src_x, src_y, info->width);
            uint32_t fb_idx = (widget->y + y) * fb_width + (widget->x + x);
            fb32[fb_idx] = color;
        }
    }
#endif
}

/**
  * @brief	图片绘制函数
  * @param	widget: 控件指针
  * @param	fb: 帧缓冲区指针
  * @retval	无
  */
static void image_draw(AkieGUI_Widget_T *widget, void *fb) {
    Image_Private *priv = (Image_Private*)widget->priv;
    
    if (!priv->img_info.data) return;  /* 没有图片数据 */
    
    if (priv->need_scale) {
        draw_image_scaled(fb, widget, priv);
    } else {
        draw_image_no_scale(fb, widget, priv);
    }
    
    widget->dirty = 0;
}

/**
  * @brief	创建图片控件
  * @param	x: 图片控件左上角 X 坐标
  * @param	y: 图片控件左上角 Y 坐标
  * @param  w: 图片控件宽度
  * @param  h: 图片控件高度
  * @param	img_info: 图片信息指针
  * @retval	AkieGUI_Widget_T实例，失败返回NULL
  */
AkieGUI_Widget_T* AkieGUI_Image_Create(
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    AkieGUI_Image_Info_T *img_info
) {
    if (g_image_count >= MAX_IMAGES) return NULL;
    
    AkieGUI_Widget_T *widget = &g_images[g_image_count].widget;
    Image_Private *priv = &g_images[g_image_count].priv;
    
    memset(widget, 0, sizeof(AkieGUI_Widget_T));
    memset(priv, 0, sizeof(Image_Private));
    
    if (img_info) {
        memcpy(&priv->img_info, img_info, sizeof(AkieGUI_Image_Info_T));
    }
    
    /* 判断是否需要缩放 */
    if (img_info) {
        if (img_info->width != w || img_info->height != h) {
            priv->need_scale = 1;
        }
    }
    
    widget->type = AKIEGUI_WIDGET_IMAGE;
    widget->x = x;
    widget->y = y;
    widget->w = w;
    widget->h = h;
    widget->state = AKIEGUI_STATE_VISIBLE | AKIEGUI_STATE_ENABLED;
    widget->dirty = 1;
    widget->draw = image_draw;
    widget->priv = priv;
    
    g_image_count++;
    return widget;
}

/**
  * @brief	更新图片内容
  * @param	img: 图片控件指针
  * @param	img_info: 新的图片信息指针
  * @retval	无
  */
void AkieGUI_Image_SetData(
    AkieGUI_Widget_T *widget,
    AkieGUI_Image_Info_T *img_info
) {
    if (!widget || widget->type != AKIEGUI_WIDGET_IMAGE || !img_info) return;
    
    Image_Private *priv = (Image_Private*)widget->priv;
    memcpy(&priv->img_info, img_info, sizeof(AkieGUI_Image_Info_T));
    
    /* 重新判断是否需要缩放 */
    priv->need_scale = 0;
    if (img_info->width != widget->w || img_info->height != widget->h) {
        priv->need_scale = 1;
    }
    
    widget->dirty = 1;
}