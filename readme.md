# AkieGUI:送给各位绘星的嵌入式极简图形库 🍁

[![License: AGPL v3](https://img.shields.io/badge/License-AGPLv3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0.html)
[![Platform: ARM Cortex-M](https://img.shields.io/badge/Platform-ARM%20Cortex--M-blue)](https://www.arm.com/zh-cn/architecture/cpu/m-profile)
[![Language: C](https://img.shields.io/badge/Language-%20C-blue)]()

![AkieGUI Logo](./assets/AkieGUI_Logo.png)
(Logo由SakuraOfficial使用Akie秋绘的形象元素创作)

## AkieGUI 是一个为嵌入式设备设计的**极简、零依赖、高可移植**的图形库。专为资源受限的MCU（STM32/RP2350/ESP32等）打造。

- [🎨 原创声明](#-原创声明)
- [⭐ 为什么叫 AkieGUI？](#-为什么叫-akiegui)
- [✨ 特性](#-特性)
- [📊 资源占用](#-资源占用)
- [🚀 快速开始](#-快速开始)
  - [1. 复制文件到你的工程](#1-复制文件到你的工程)
  - [2. 配置 `akiegui_config.h`](#2-配置-akiegui_configh)
  - [3. 实现屏幕驱动 (唯一必须写的代码)](#3-实现屏幕驱动-唯一必须写的代码)
- [📖 API 文档](#-api-文档)
  - [内存管理 API](#内存管理-api)
- [图形核心 API](#图形核心-api)
- [🔧 移植指南](#-移植指南)
  - [步骤1：实现发送函数](#步骤1实现发送函数)
  - [步骤2：处理TE信号（可选）](#步骤2处理te信号可选)
- [🎯 示例场景](#-示例场景)
  - [场景1：SPI彩屏 + DMA（主流方案）](#场景1spi彩屏--dma主流方案)
  - [场景2：SPI彩屏 + BDMA（STM32H7 D3域）](#场景2spi彩屏--bdmastm32h7-d3域)
  - [场景3：无DMA，轮询发送（低端MCU）](#场景3无dma轮询发送低端mcu)
  - [场景4：FreeRTOS多任务](#场景4freertos多任务)
- [📈 性能优化建议](#-性能优化建议)
- [✅ 已知问题 \& 解决方案](#-已知问题--解决方案)
- [📄 开源协议](#-开源协议)
- [⚖️ AGPLv3 核心要求](#️-agplv3-核心要求)
  - [✅ 允许的（免费，无需联系我）：](#-允许的免费无需联系我)
  - [❌ 不允许的（别这么干）：](#-不允许的别这么干)
- [🤝 贡献指南](#-贡献指南)
  - [首次贡献？没问题！ 可以从以下方面入手：](#首次贡献没问题-可以从以下方面入手)
- [🍁 特别致谢](#-特别致谢)
- [📮 联系方式](#-联系方式)
- [📢 违规使用举报通道](#-违规使用举报通道)

> [!IMPORTANT]
> ### 当前阶段和实现的功能？
> 当前在还未实际上机进行测试的Native阶段，实现了内存管理和显存发送操作。
>
> 请注意，代码示例里有部分API是目前未实现的，需要等待后续的测试后更新。
> 

## 🎨 原创声明

**AkieGUI 是 100% 原创设计，未参考任何现有图形库。**

它不是 LittlevGL 的简化版，不是 U8g2 的移植，也不是 emWin 的平替。

AkieGUI 的每一行代码都来自实际项目中的痛点：
- BDMA 无法直接访问 SDRAM？
- SRAM3 小得可怜？
- 只想画个屏却要配置一堆复杂结构体？

所以有了 AkieGUI —— **为真实问题而生，为极简而写。**

## ⭐ 为什么叫 AkieGUI？

**AkieGUI** 的名字来源于 B站虚拟唱见 [**Akie秋绘**](https://space.bilibili.com/4176573)：

- 🍁 **秋** - 秋天，收获的季节（愿你用这个库也能收获成果）
- 🎨 **绘** - 绘画，图形库的本职工作
- 🎵 **秋绘** - 温暖清澈的歌声，陪伴我写完每一行代码

写代码的时候一直循环秋绘的歌单(同时作为2027的粉丝二创)，所以这个库就叫 AkieGUI 了。

## ✨ 特性

- 🎯 **极简设计** - 核心仅2个文件，无复杂抽象，易读易改
- 💾 **内存自主** - 内置内存池管理器，不依赖malloc/free
- 🔄 **双缓冲** - 可选双缓冲，彻底消除撕裂
- ⚡ **TE感知** - 支持任何形式的传输完成通知（DMA/GPIO/轮询）
- 🧩 **高度可移植** - 只需实现`send_frame`一个函数
- 🔧 **FreeRTOS就绪** - 一个宏开关，无缝切换
- 🚫 **零动态内存** - 静态内存池，运行时确定

## 📊 资源占用

| 配置 | FLASH | RAM | 帧缓冲 |
|------|-------|-----|--------|
| 核心库 | ~2KB | ~40字节 | 外部管理 |
| 320x240 RGB565 | +0 | 150KB | 用户分配 |
| 800x480 RGB565 | +0 | 750KB | 用户分配 |

**总占用**：核心库 + 公共组件 + 字库 ≈ **55KB Flash**

## 🚀 快速开始

### 1. 复制文件到你的工程

    your_project/
    ├──AkieGUI/
    |   ├── Core/                      # 核心层
    |   │   ├── Inc/
    |   │   │   ├── akiegui_core.h
    |   │   │   └── akiegui_memory.h
    |   │   └── Src/
    |   │       ├── akiegui_core.c
    |   │       └── akiegui_memory.c
    |   │
    |   ├── Common/                    # 公共组件
    |   │   ├── Inc/
    |   │   │   ├── akiegui_port.h     # 移植层
    |   │   │   ├── akiegui_color.h    # 颜色转换
    |   │   │   └── akiegui_draw.h     # 绘制函数
    |   │   └── Src/
    |   │       └── akiegui_draw.c
    |   │
    |   ├── Widget/                    # 控件层
    |   │   ├── akiegui_widget.h
    |   │   ├── akiegui_widget.c
    |   │   ├── Button/
    |   │   │   ├── akiegui_button.h
    |   │   │   └── akiegui_button.c
    |   │   └── Image/
    |   │   │   ├── akiegui_image.h
    |   │   │   └── akiegui_image.c
    |   │   └── Label/
    |   │       ├── akiegui_label.h
    |   │       └── akiegui_label.c
    |   │    
    |   ├── Fonts/                      # 字库
    |   │   ├── akiegui_font_ascii.h
    |   │   └── akiegui_font_ascii.c
    |   │
    |   ├── akiegui_config.h
    |   └── akiegui.h
    └── main.c

### 2. 配置 `akiegui_config.h`

```c
/* 选择内存管理方式 */
#define AkieGUI_USE_FREERTOS 0   /* 0=裸机内存池, 1=FreeRTOS堆 */

/* 选择TE模式 */
#define AkieGUI_KE_NO_TE 0  // 非TE模式
#define AkieGUI_KE_TE_EN 1  // 启用TE等待

#define AkieGUI_TE_MODE     AkieGUI_KE_NO_TE

/* 选择显存位置 */
#define AkieGUI_MEM_TYPE    AkieGUI_MEM_INTERNAL /* 0=内部RAM, 1=外部SDRAM */
```

### 3. 实现屏幕驱动 (唯一必须写的代码)
```c
#include "akiegui.h"

/* SPI DMA发送完成中断 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi6) {
        AkieGUI_TransmitEnd();  /* 通知图形库传输完成 */
    }
}

/* 整帧发送函数（你必须实现）*/
void my_send_frame(uint8_t *data, uint32_t len) {
    HAL_SPI_Transmit_IT(&hspi6, data, len);  /* 你的发送代码 */
}

int main(void) {
    /* 1. 初始化内存池（裸机版）*/
    static uint8_t mem_pool[512 * 1024];
    AkieGUI_MemInit(mem_pool, sizeof(mem_pool));
    
    /* 2. 注册驱动 */
    g_akiegui.send_frame = my_send_frame;
    
    /* 3. 初始化帧缓冲（320x240 RGB565 双缓冲）*/
    AkieGUI_FBInit();
    
    /* 4. 绘制红色 */
    memset(AkieGUI_GetDrawFB(), 0xF800, g_akiegui.fb_size);
    
    /* 5. 提交到屏幕 */
    AkieGUI_Commit();
#if AKIEGUI_TE_MODE == AkieGUI_KE_TE_EN
    AkieGUI_WaitTE();  /* 等待发送完成 */
#endif
    AkieGUI_SwapBuffer(); /* 双缓冲交换 */
    
    while(1) {
        /* 你的应用代码 */
    }
}
```

## 📖 API 文档
### 内存管理 API

| 函数 | 描述 |
|------|------|
| `AkieGUI_MemInit(pool, size)` | 初始化内存池（裸机版）|
| `AkieGUI_MemAlloc(size)` | 分配内存（默认16字节对齐）|
| `AkieGUI_MemAllocAlign(size, align)` | 分配对齐内存 |
| `AkieGUI_MemCalloc(nmemb, size)` | 分配并清零 |
| `AkieGUI_MemFree(ptr)` | 释放内存 |
| `AkieGUI_MemGetFree()` | 获取空闲内存大小 |
| `AkieGUI_MemGetUsed()` | 获取已用内存大小 |

#### 颜色工具 (akiegui_color.h)
| 函数 | 描述 |
|------|------|
| `akiegui_rgb888_to_native(rgb)` | RGB888 → 本地颜色格式（自动适配 BPP）|
| `akiegui_native_to_rgb888(color)` | 本地颜色 → RGB888 |
| `akiegui_color_t` | 根据 BPP 自动适配的颜色类型 |

预定义颜色（RGB888格式）：
```c
AKIEGUI_RED      0xFF0000
AKIEGUI_GREEN    0x00FF00
AKIEGUI_BLUE     0x0000FF
AKIEGUI_WHITE    0xFFFFFF
AKIEGUI_BLACK    0x000000
```

#### 绘制函数 (akiegui_draw.h)
| 函数 | 描述 |
|------|------|
| `akiegui_draw_rect(fb, x, y, w, h, color)` | 绘制矩形 |
| `akiegui_draw_char(fb, x, y, ch, color, bg, transparent, font)` | 绘制单个字符 |
| `akiegui_draw_string(fb, x, y, str, color, bg, transparent, font)` | 绘制字符串 |
| `akiegui_text_width(str, font)` | 计算字符串宽度 |

## 🧩 控件基类 API

| 函数 | 描述 |
|------|------|
| `AkieGUI_Widget_Init()` | 初始化控件系统 |
| `AkieGUI_Widget_Add(widget)` | 添加控件到管理器 |
| `AkieGUI_Widget_Remove(widget)` | 从管理器移除控件 |
| `AkieGUI_Widget_DrawDirtyAll()` | 绘制所有脏控件并提交 |
| `AkieGUI_Widget_RedrawAll()` | 强制重绘所有控件 |
| `AkieGUI_Widget_MarkDirty(widget)` | 标记控件需要重绘 |
| `AkieGUI_Widget_HitTest(x, y)` | 命中测试，返回坐标上的控件 |

## 🎯 已实现的控件 API

| 函数 | 描述 |
|------|------|
| `AkieGUI_Button_Create(x, y, w, h, text, text_color, bg_color, press_color)` | 创建按钮（颜色用RGB888）|
| `AkieGUI_Button_SetFont(btn, font)` | 设置按钮字体（pFONT*）|
| `AkieGUI_Button_SetText(btn, text)` | 设置按钮文字 |
| `AkieGUI_Button_SetColors(btn, text_color, bg_color, press_color)` | 设置按钮颜色 |

## 图形核心 API
| 函数 | 描述  | 
|-------------|----------------|
| `AkieGUI_FBInit()` | 初始化帧缓冲 |
| `AkieGUI_GetDrawFB()` |	获取当前绘制缓冲区 |
| `AkieGUI_GetDispFB()` | 获取当前显示缓冲区 |
| `AkieGUI_SwapBuffer()` | 交换双缓冲 |
| `AkieGUI_Commit()` | 提交整帧到屏幕 |
| `AkieGUI_CommitRegion(x, y, w, h)` | 提交区域到屏幕 (需实现 send_region 驱动) |
| `AkieGUI_SendFrame(data, len)` | 发送帧（内部调用） |
| `AkieGUI_WaitTE()` | 等待传输完成 |
| `AkieGUI_TransmitEnd()` | 必须在中断调用！通知传输完成 |

## 已实现的控件 API
| 控件 | 函数 | 描述 |
|------|------|------|
| **按钮** | `AkieGUI_Button_Create(x, y, w, h, text, text_color, bg_color, press_color)` | 创建按钮 |
| | `AkieGUI_Button_SetFont(btn, font)` | 设置按钮字体 |
| | `AkieGUI_Button_SetText(btn, text)` | 设置按钮文字 |
| | `AkieGUI_Button_SetColors(btn, text_color, bg_color, press_color)` | 设置按钮颜色 |
| **标签** | `AkieGUI_Label_Create(x, y, text, text_color, bg_color, font)` | 创建标签 |
| | `AkieGUI_Label_SetText(label, text)` | 设置标签文字 |
| | `AkieGUI_Label_SetColor(label, text_color)` | 设置标签颜色 |
| | `AkieGUI_Label_SetBgColor(label, bg_color)` | 设置标签背景色（0xFFFF00=透明）|
| **图片** | `AkieGUI_Image_Create(x, y, w, h, img_info)` | 创建图片控件 |
| | `AkieGUI_Image_SetData(img, img_info)` | 更新图片数据 |

### 图片格式
```c
/* 图片信息结构体 - 数据格式固定为 RGB888 */
typedef struct {
    uint16_t width;             /* 图片宽度（像素）*/
    uint16_t height;            /* 图片高度（像素）*/
    const void* data;        /* RGB888 数据指针 */
    uint32_t data_size;         /* 数据大小（字节）*/
} AkieGUI_Image_Info_T;
```

### 控件API示例
```c
/* 创建按钮 */
AkieGUI_Widget_T *btn = AkieGUI_Button_Create(
    100, 100, 120, 40, "Click",
    AKIEGUI_WHITE,     /* 白字 */
    AKIEGUI_BLUE,      /* 蓝底 */
    0x000080           /* 深蓝按下 */
);

/* 创建标签 */
AkieGUI_Widget_T *label = AkieGUI_Label_Create(
    10, 10, "Hello",
    AKIEGUI_RED,       /* 红字 */
    AKIEGUI_TRANSPARENT, /* 透明背景 */
    &ASCII_8x16
);

/* 创建图片控件 */

/* 定义一张 100x100 的 RGB888 图片 */
const uint8_t my_image[] = {
    0xFF, 0x00, 0x00,  /* 第一个像素：红 */
    0x00, 0xFF, 0x00,  /* 第二个像素：绿 */
    0x00, 0x00, 0xFF,  /* 第三个像素：蓝 */
    // ... 更多像素
};

AkieGUI_Image_Info_T img_info = {
    .width = 100,
    .height = 100,
    .data = my_image,
    .data_size = sizeof(my_image)
};

/* 创建图片控件（显示在 120x120 区域，自动缩放）*/
AkieGUI_Widget_T *img = AkieGUI_Image_Create(10, 10, 120, 120, &img_info);

/* 添加到管理器 */
AkieGUI_Widget_Add(btn);
AkieGUI_Widget_Add(label);
AkieGUI_Widget_Add(img);

/* 更新图片内容 */
AkieGUI_Image_SetData(img, &new_img_info);
```

## 🔧 移植指南
### 步骤1：实现发送函数
```c
/* 必须实现 - 整帧发送 */
void my_send_frame(uint8_t *data, uint32_t len) {
    /* 你的硬件发送代码 */
}

/* 可选实现 - 区域发送（性能优化）*/
void my_send_region(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    LCD_SetWindow(x, y, x+w-1, y+h-1);
    my_send_frame(data, w * h * (g_akiegui.fb_bpp/8));
}
```
### 步骤2：处理TE信号（可选）
```c
/* 任何传输完成中断里调用 */
void DMA2D_IRQHandler(void) {
    AkieGUI_TransmitEnd();
}

void EXTI9_5_IRQHandler(void) {  /* TE引脚中断 */
    AkieGUI_TransmitEnd();
}

void BDMA_Channel0_IRQHandler(void) {
    AkieGUI_TransmitEnd();
}
```

## 🎯 示例场景
### 场景1：SPI彩屏 + DMA（主流方案）
```c
void my_send_frame(uint8_t *data, uint32_t len) {
    HAL_SPI_Transmit_DMA(&hspi6, data, len);
}
```
### 场景2：SPI彩屏 + BDMA（STM32H7 D3域）
```c
static uint8_t sram3_buf[1024] __attribute__((section(".sram3")));

void my_send_frame(uint8_t *data, uint32_t len) {
    /* 分块传输：SDRAM → SRAM3 → BDMA → SPI */
    for(uint32_t i = 0; i < len; i += 1024) {
        uint32_t chunk = min(1024, len - i);
        memcpy(sram3_buf, data + i, chunk);
        HAL_DMA_Start_IT(&hbdma0, sram3_buf, &hspi6.Instance->DR, chunk);
    }
}
```
### 场景3：无DMA，轮询发送（低端MCU）
```c
void my_send_frame(uint8_t *data, uint32_t len) {
    HAL_SPI_Transmit(&hspi6, data, len, HAL_MAX_DELAY);
    AkieGUI_TransmitEnd();  /* 立即通知完成 */
}
```
### 场景4：FreeRTOS多任务
```c
/* akiegui_config.h */
#define AKIEGUI_USE_FREERTOS 1  /* 切换到FreeRTOS */

/* 任务1：绘制动画 */
void task_draw(void *arg) {
    while(1) {
        draw_ui(AkieGUI_GetDrawFB());
        AkieGUI_Commit();
        vTaskDelay(16);
    }
}

/* 任务2：处理触摸 */
void task_touch(void *arg) {
    while(1) {
        /* 直接使用AkieGUI_MemAlloc，线程安全 */
        void *buf = AkieGUI_MemAlloc(1024);
        AkieGUI_MemFree(buf);
    }
}
```

> [!NOTE]
> ## 以下代码逻辑已写完，但作者人在外地暂无法实测
> 如果你手头有 RGB 屏幕（STM32H7/F7 系列 + LTDC），欢迎帮忙验证。
> 遇到问题可以提 Issue，附上调试信息，咱们一起修。
> 
> 代码放这里不是为了坑人，是为了：
> 1. 给有同样需求的人一个参考
> 2. 等 PR 或者作者回去拿屏幕配硬件来完善它
> ### 场景5：LTDC + DMA2D（RGB接口高速屏）
> ```c
> static volatile uint8_t dma2d_busy = 0;
>
> /* STM32H7/F7 系列，800x480 ARGB8888，DMA2D加速 */
> void my_send_frame(uint8_t *data, uint32_t len) {
>     (void)len;
>     /* 清理无效的数据缓存 */
>     /* 如果MCU有D-Cache，需要做Cache维护。如果无Cache，可注释掉 */
>     SCB_CleanInvalidateDCache();
>     /* 配置DMA2D寄存器（直接操作寄存器，无HAL开销）*/
>     DMA2D->CR = 0x00000000UL | (1 << 9);                            /* 内存到内存模式 */
>     DMA2D->FGMAR = (uint32_t)data;                                  /* 源地址（LVGL绘制缓冲区）*/
>     DMA2D->OMAR = AkieGUI_MEM_ADDR;                                 /* 目标地址（LTDC显存）*/
>     DMA2D->FGOR = 0;                                                /* 源行偏移 */
>     DMA2D->OOR = 0;                                                 /* 目标行偏移 */
>     DMA2D->FGPFCCR = DMA2D_OUTPUT_ARGB8888;                         /* 32位颜色格式 */
>     DMA2D->OPFCCR = DMA2D_OUTPUT_ARGB8888;
>     DMA2D->NLR = (AkieGUI_LCD_WIDTH << 16) | AkieGUI_LCD_HEIGHT;    /* 行列数 */
>     DMA2D->CR |= DMA2D_IT_TC;                                       /* 开启传输完成中断 */
>     DMA2D->CR |= DMA2D_CR_START;                                    /* 启动传输 */
>     
>     dma2d_busy = 1;
> }
> 
> /* 请注册一个或使用现有的DMA2D传输完成回调函数 */
> void mDMA2DCpltCallback(DMA2D_HandleTypeDef *hdma2d) {
>     if (hdma2d->Instance==DMA2D)
>     {
>         if(dma2d_busy == 1)
>         {
>             AkieGUI_TransmitEnd();                                  /* 通知传输完成 */
>             dma2d_busy = 0;
>         }
>     }
> }
> ```
> 
> ### 场景6：LTDC + 双缓冲 + TE（零撕裂）
> ```c
> /* send_frame 函数*/
> void my_send_frame(uint8_t *data, uint32_t len) {
>     (void)len;
>     LTDC_Layer1->CFBAR = (uint32_t)data;			// 切换显存地址
> }
> 
> /* 利用LTDC的行事件中断切换显存地址 */
> void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc) {
>     __HAL_LTDC_RELOAD_CONFIG(hltdc);
>     HAL_LTDC_ProgramLineEvent(hltdc, 0);
>     AkieGUI_TransmitEnd();      /* 通知图形库 */
> }
> ```
> 这两种遇到问题也别慌~可以插点断点排查
>
> 提 Issue 时请附上：
> - MCU 型号
> - 屏幕规格（分辨率/接口）
> - 调试输出（如果有）
> 
> 写出这些信息可以让 Issue 质量更高，也方便开发者复现调试

## 📈 性能优化建议
1. **使用双缓冲** + `AkieGUI_SwapBuffer()` - 彻底消除撕裂

2. **实现**`send_region` - 只更新变化区域

3. **启用TE模式** - 等待VSYNC，避免半帧

4. **对齐32字节** - 配合DMA2D/BDMA性能最佳

5. **内存池大小** - 帧缓冲2倍 + 20%余量

## ✅ 已知问题 & 解决方案
| 问题 | 原因 | 解决 |
|------|-----|------|
|屏幕不亮 | send_frame未实现 | 检查函数指针是否赋值|
|花屏 | 帧缓冲大小错误/颜色格式不匹配 | 检查fb_bpp和fb_size/AkieGUI_LCD_BPP赋值 |
|卡死在WaitTE | 未调用TransmitEnd | 检查中断是否正确调用 |
|内存分配失败 | 内存池太小 | 增大内存池或减少双缓冲 |

## 📄 开源协议
**AkieGUI ©** 2026 [雪琳Sherlyn/Xuelin-Sherlyn]

本软件采用 **GNU Affero General Public License v3.0 (AGPLv3)** 协议发布。
```text
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
```

## ⚖️ AGPLv3 核心要求
### ✅ 允许的（免费，无需联系我）：
- 个人学习、研究

- 毕设/课程设计（必须开源、必须注明来源）

- 开源项目（必须同样以AGPLv3开源）

- 商业产品（必须开源你的修改）

- 网络服务（必须提供源代码）

### ❌ 不允许的（别这么干）：
- 删掉版权声明

- 不提供源代码就分发

- 闭源商用（AGPLv3要求必须开源，因此闭源商用不被允许）

**有bug自己修哈，我做的是代码，不是服务。**  
但如果能把bug现象和修复过程发到Issues也可以帮到不少人

## 🤝 贡献指南
1. Fork 本仓库

2. 创建你的特性分支 (git checkout -b feature/AmazingFeature)

3. 提交你的修改 (git commit -m 'Add some AmazingFeature')

4. 推送到分支 (git push origin feature/AmazingFeature)

5. 提交 Pull Request

### 首次贡献？没问题！ 可以从以下方面入手：

- 帮忙测试未验证的硬件场景（LTDC/DMA2D/FSMC）

- 补充更多移植示例

- 完善注释或文档

**注意**：所有贡献的代码也将采用 AGPLv3 协议。

## 🍁 特别致谢

感谢虚拟UP主 [@Akie秋绘](https://space.bilibili.com/4176573) 授权使用其形象元素。
秋绘的歌声陪伴了无数个debug的深夜，那片红枫叶也成为了这个项目的灵魂。

本项目Logo及启动画面设计均获得官方授权，请勿在其他项目中使用。

## 📮 联系方式
- 作者：[雪琳Sherlyn/Xuelin-Sherlyn]

- 哔哩哔哩: [雪琳Sherlyn个人主页](https://space.bilibili.com/1815675515)

- 邮箱：[xuelin-sherlyn@outlook.com](mailto:xuelin-sherlyn@outlook.com)

- GitHub：https://github.com/Xuelin-Sherlyn/AkieGUI

- Issues：https://github.com/Xuelin-Sherlyn/AkieGUI/issues

> [!CAUTION]
> ## ⚠️ 重要提醒（必读）
> 
> **AkieGUI 的开发者们不是外包团队，也不是免费劳动力，更不是免费客服。**
> 
> ### 如果你只是学习：
> - ✅ 随便用，随便改
> - ✅ 代码里注释写得很清楚
> - ✅ 出bug自己修，别来找我
> 
> ### 如果你想用在产品里：
> - ✅ 可以，**AGPLv3 允许免费商用**
> - ✅ 但必须开源你的修改
> - ✅ 必须保留版权声明
> - ⚠️ **本项目为个人兴趣维护，不提供商业技术支持服务。如需商业合作或定制开发，请通过邮件联系，我们视时间和精力决定是否承接。**
> - 回复可能很慢（毕竟主业不是干这个），急着要的请绕道。
> 
> ### 关于"背锅"：
> AGPLv3 第15-16条写得明明白白：
> - 代码按"原样"提供，**没有任何保证**
> - 作者**不对任何损失负责**
> - 用了出问题是你自己的事
> 
> **用了就是同意，不同意就别用。**
> 
> ## 🔧 关于 NDA 芯片的使用说明
> 
> AkieGUI 采用 AGPLv3 协议，原则上要求所有使用都必须开源。
> 
> ### 🎯 但我知道有些芯片受 NDA 保护
> - 车规芯片、专用IC、未发布的新品
> - 一旦涉及，**整个项目都不能公开**
> 
> ### 📢 作为作者，我的态度：
> - ✅ **我不会主动追究 NDA 芯片上的使用**
> - ✅ 你不需要开源 NDA 相关代码
> 
> ### ⚠️ 只有一个要求：**告诉我一声**
> 发个邮件到 [xuelin-sherlyn@outlook.com](mailto:xuelin-sherlyn@outlook.com)，简单说：
> - "我在用 AkieGUI，项目涉及 NDA 芯片"
> - 不需要具体芯片型号，不需要项目细节
> 
> **我收到邮件就会标记：这个项目我知道，不追究。**
> 
> ### ⚠️ 为什么必须告知？
> 因为其他贡献者可能追究，我需要帮你挡。
> 
> ### 🤝 给贡献者的建议：
> 发现没开源的 NDA 项目，先查我有没有收到告知邮件。
> 查不到再发 Issue，别直接举报。
> 
> **我们是一个社区，目标是让更多人学会嵌入式，不是为了互相举报。**
> 
> —— 雪琳Sherlyn

## 📢 违规使用举报通道

如果你发现有人在以下场景使用了 AkieGUI 但未遵守 AGPLv3：
- B站/YouTube 视频
- 开源项目
- 培训机构
- 商业产品

请在本仓库提交 Issue，提供：
1. 违规链接
2. 证据截图
3. 是否已提醒

我们会：
✅ 公开记录（给后来者警示）
✅ 联系对方提醒
✅ 必要时向平台投诉

**保护开源，需要大家一起监督。**
