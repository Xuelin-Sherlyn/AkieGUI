/* ============= akiegui_color.h ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 字体基类部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#pragma once
#include <stdint.h>

// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//↑这里有空格

#define FONT_TYPE_ASCII 437
#define FONT_TYPE_GBK   936

typedef struct _pFont
{    
	const uint8_t 		*pTable;  		//  字模数组地址
	uint16_t 			Width; 		 	//  单个字符的字模宽度
	uint16_t 			Height; 		//  单个字符的字模长度
	uint16_t 			Sizes;	 		//  单个字符的字模数据个数
	uint16_t			Table_Rows;		//  该参数只有汉字字模用到，表示二维数组的行大小
    uint16_t            FontType;       //  字体标识符
} pFONT;