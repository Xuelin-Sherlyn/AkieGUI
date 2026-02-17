/* ============= akiegui_core.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 图形核心头文件
 * 
 * 定义核心数据结构和内联API：
 *   - AkieGUI_t 主结构体
 *   - TE同步机制
 *   - 帧缓冲操作
 *   - 提交/交换缓冲区
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_CORE_H__
#define __AKIEGUI_CORE_H__

#include "akiegui_config.h"
#include "akiegui_memory.h"

/* ============= 图形库主结构 ============= */
typedef struct {
    /* ----- TE同步（唯一硬件依赖）----- */
    #if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    volatile uint8_t te_pending;    /* 传输等待标志 */
    #endif
    
    /* ----- 屏幕驱动接口（用户实现）----- */
    void (*send_frame)(uint8_t *data, uint32_t len);
    void (*send_region)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
    
    /* ----- 帧缓冲管理 ----- */
    uint8_t *fb1;          /* 缓冲区1 */
    uint8_t *fb2;          /* 缓冲区2（双缓冲）*/
    uint8_t *draw_fb;      /* 当前绘制缓冲区 */
    uint8_t *disp_fb;      /* 当前显示缓冲区 */
    uint32_t fb_width;     /* 帧缓冲宽度 */
    uint32_t fb_height;    /* 帧缓冲高度 */
    uint32_t fb_bpp;       /* 每像素位数 */
    uint32_t fb_size;      /* 单缓冲区大小 */
    uint8_t  double_buffer;/* 是否启用双缓冲 */
    
    /* ----- 屏幕信息 ----- */
    uint16_t screen_width;
    uint16_t screen_height;
    uint8_t  screen_bpp;
    
    /* ----- 用户自定义 ----- */
    void *user_data;
} AkieGUI_t;

/* 全局实例 */
extern AkieGUI_t g_akiegui;

/* ============= TE同步API（编译期优化）============= */

/**
 * @brief 用户必须在传输完成中断中调用此函数！
 *        无论DMA2D、BDMA、SPI、TIMER、GPIO中断，只要传输完成就调它
 */
static inline void AkieGUI_TransmitEnd(void) {
#if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    g_akiegui.te_pending = 0;
#else
    /* 无TE模式：空操作，编译优化掉 */
#endif
}

/**
 * @brief 用户发送帧时调用此函数
 */
static inline void AkieGUI_SendFrame(uint8_t *data, uint32_t len) {
#if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    g_akiegui.te_pending = 1;  /* 置位等待标志 */
#endif
    if (g_akiegui.send_frame) {
        g_akiegui.send_frame(data, len);
    }
}

/**
 * @brief 用户发送区域时调用此函数
 */
static inline void AkieGUI_SendRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
#if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    g_akiegui.te_pending = 1;
#endif
    if (g_akiegui.send_region) {
        g_akiegui.send_region(x, y, w, h, data);
    }
}

/**
 * @brief 等待上一帧传输完成
 */
static inline void AkieGUI_WaitTE(void) {
#if AkieGUI_TE_MODE == AkieGUI_KE_TE_EN
    while (g_akiegui.te_pending);
#endif
}

/* ============= 帧缓冲API ============= */

/**
 * @brief 初始化帧缓冲
 * @param width     屏幕宽度
 * @param height    屏幕高度
 * @param bpp       每像素位数(16/24/32)
 * @param double_buf 是否使用双缓冲
 * @return 0成功 -1失败
 */
int AkieGUI_FBInit(void);

/**
 * @brief 获取当前绘制缓冲区
 */
static inline uint8_t* AkieGUI_GetDrawFB(void) {
    return g_akiegui.draw_fb;
}

/**
 * @brief 获取当前显示缓冲区
 */
static inline uint8_t* AkieGUI_GetDispFB(void) {
    return g_akiegui.disp_fb;
}

/**
 * @brief 交换缓冲区（双缓冲模式）
 */
static inline void AkieGUI_SwapBuffer(void) {
    if (g_akiegui.double_buffer) {
        uint8_t *tmp = g_akiegui.draw_fb;
        g_akiegui.draw_fb = g_akiegui.disp_fb;
        g_akiegui.disp_fb = tmp;
    }
}

/**
 * @brief 提交整帧到屏幕
 */
static inline int AkieGUI_Commit(void) {
    AkieGUI_SendFrame(g_akiegui.draw_fb, g_akiegui.fb_size);
    return 0;
}

/**
 * @brief 提交区域到屏幕
 */
static inline int AkieGUI_CommitRegion(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    /* 参数检查 */
    if (x >= g_akiegui.fb_width || y >= g_akiegui.fb_height) return -1;
    if (x + w > g_akiegui.fb_width) w = g_akiegui.fb_width - x;
    if (y + h > g_akiegui.fb_height) h = g_akiegui.fb_height - y;
    if (w == 0 || h == 0) return 0;
    
    uint32_t bytes_per_pixel = g_akiegui.fb_bpp / 8;
    uint32_t stride = g_akiegui.fb_width * bytes_per_pixel;
    uint32_t offset = (y * g_akiegui.fb_width + x) * bytes_per_pixel;
    // uint32_t line_bytes = w * bytes_per_pixel;
    
    for (uint16_t i = 0; i < h; i++) {
        /* 逐行发送，line_bytes表示每行数据大小 */
        AkieGUI_SendRegion(x, y + i, w, 1, 
                          g_akiegui.draw_fb + offset + i * stride);
    }
    return 0;
}

#endif