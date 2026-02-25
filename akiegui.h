/* ============= akiegui.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 主头文件
 * 
 * 包含全部API：
 *   - 内存管理
 *   - 帧缓冲操作
 *   - TE同步控制
 * 
 * 用户程序只需 #include "akiegui.h"
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#ifndef __AKIEGUI_H__
#define __AKIEGUI_H__

/* 单头文件包含全部 */
#include "akiegui_config.h"
#include "akiegui_memory.h"
#include "akiegui_core.h"
#include "akiegui_widget.h"
#include "akiegui_button.h"

/* 版本信息 */
#define AKIEGUI_VERSION_MAJOR    0
#define AKIEGUI_VERSION_MINOR    1
#define AKIEGUI_VERSION_PATCH    0

#endif