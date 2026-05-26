/* ============= akiegui_draw.c ============= */
/*
 * AkieGUI - 嵌入式极简图形库
 * Copyright (C) 2026 雪琳Sherlyn (Xuelin-Sherlyn)
 *
 * 图形绘制部分
 *
 * 许可证: AGPL v3 (看许可证文件)
 * 联系方式: xuelin-sherlyn@outlook.com
 * B站: https://space.bilibili.com/1815675515
 */
#include "akiegui_draw.h"
#include <string.h>

/**
  * @brief	矩形绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 矩形坐标 X
  *	@param	y: 矩形坐标 Y
  *	@param	w: 矩形宽度
  *	@param	h: 矩形高度
  *	@param	color: 矩形颜色
*/

void akiegui_draw_rect(
    void *fb,
    uint16_t x, uint16_t y,
    uint16_t w, uint16_t h,
    akiegui_color_t color
) {
    uint16_t fb_width = g_akiegui.fb_width;
    
#if AkieGUI_LCD_BPP == 16
    uint16_t *fb16 = (uint16_t*)fb;
    for (uint16_t row = 0; row < h; row++) {
        uint32_t base = (y + row) * fb_width + x;
        for (uint16_t col = 0; col < w; col++) {
            fb16[base + col] = color;
        }
    }
#elif AkieGUI_LCD_BPP == 24 || AkieGUI_LCD_BPP == 32
    uint32_t *fb32 = (uint32_t*)fb;
    for (uint16_t row = 0; row < h; row++) {
        uint32_t base = (y + row) * fb_width + x;
        for (uint16_t col = 0; col < w; col++) {
            fb32[base + col] = color;
        }
    }
#endif
}

/**
  * @brief	字符绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 字符坐标 X
  *	@param	y: 字符坐标 Y
  * @param  ch: 字符
  *	@param	color: 字符颜色
  *	@param	bg_color: 背景颜色
  * @param  transparent: 背景是否透明
  * @param  font: 使用的字体
*/
void akiegui_draw_char(
    void *fb,
    uint16_t x, uint16_t y,
    char ch,
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *font
) {
    if (!font || !font->pTable || ch < 32 || ch > 126) return;
    
    uint16_t fb_width = g_akiegui.fb_width;
    uint16_t offset = (ch - 32) * font->Sizes;
    uint8_t bytes_per_row = (font->Width + 7) / 8;
    
#if AkieGUI_LCD_BPP == 16
    uint16_t *fb16 = (uint16_t*)fb;
    for (uint8_t row = 0; row < font->Height; row++) {
        for (uint8_t byte = 0; byte < bytes_per_row; byte++) {
            uint8_t data = font->pTable[offset + row * bytes_per_row + byte];
            uint8_t start_col = byte * 8;
            
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t col = start_col + bit;
                if (col >= font->Width) break;
                
                uint32_t fb_idx = (y + row) * fb_width + (x + col);
                if (data & (1 << (7 - bit))) {
                    fb16[fb_idx] = color;
                } else if (!transparent) {
                    fb16[fb_idx] = bg_color;
                }
            }
        }
    }
#elif AkieGUI_LCD_BPP == 24 || AkieGUI_LCD_BPP == 32
    uint32_t *fb32 = (uint32_t*)fb;
    for (uint8_t row = 0; row < font->Height; row++) {
        for (uint8_t byte = 0; byte < bytes_per_row; byte++) {
            uint8_t data = font->pTable[offset + row * bytes_per_row + byte];
            uint8_t start_col = byte * 8;
            
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t col = start_col + bit;
                if (col >= font->Width) break;
                
                uint32_t fb_idx = (y + row) * fb_width + (x + col);
                if (data & (1 << (7 - bit))) {
                    fb32[fb_idx] = color;
                } else if (!transparent) {
                    fb32[fb_idx] = bg_color;
                }
            }
        }
    }
#endif
}

/**
  * @brief	中文字符绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 字符坐标 X
  *	@param	y: 字符坐标 Y
  * @param  ch: GB2312字符
  *	@param	color: 字符颜色
  *	@param	bg_color: 背景颜色
  * @param  transparent: 背景是否透明
  * @param  font: 使用的字体
*/
void akiegui_draw_chinese_char(
    void *fb,
    uint16_t x, uint16_t y,
    const char *ch,             /* GB2312 双字节 */
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *font                 /* 中文字体 */
){
    if (ch == NULL || font == NULL) return;
    // 字符编码到字库索引的映射
    uint16_t char_index = 0;
    uint16_t fb_index = 0;
#if AkieGUI_LCD_BPP == 16
    uint16_t *fb16 = (uint16_t*)fb;
    while(1)
	{		
		// 通过对比数组中的汉字编码，定位字模地址
		if ( *(font->pTable + (fb_index+1)*font->Sizes + 0)==*ch && *(font->pTable + (fb_index+1)*font->Sizes + 1)==*(ch+1) )	
		{
		  char_index=fb_index;	// 字模地址偏移
		  break;
		}				
		fb_index+=2;	// 每个中文字符占两字节

		if(fb_index >= font->Table_Rows)	return;	// 字模列表没这个字
	}
    fb_index = 0;
    // 计算字模数据偏移量
    const uint8_t* char_data = font->pTable + char_index * font->Sizes;
    // 计算每行需要的字节数
    uint8_t bytes_per_row = (font->Width + 7) / 8;
    // 遍历每一行
    for (uint8_t row = 0; row < font->Height; row++) {
        for (uint8_t byte_idx = 0; byte_idx < bytes_per_row; byte_idx++) {
            uint8_t line_byte = char_data[row * bytes_per_row + byte_idx];
            uint8_t start_col = byte_idx * 8;

            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t col = start_col + bit;
                if (col >= font->Width) break;

                fb_index = (y + row) * g_akiegui.fb_width + (x + col);
                if (line_byte & (1 << (7 - bit))) {
                   fb16[fb_index] = color;
                } else if (!transparent) {
                    fb16[fb_index] = bg_color;
                }
            }
        }
    }
#elif AkieGUI_LCD_BPP == 24 || AkieGUI_LCD_BPP == 32
    uint32_t *fb32 = (uint32_t*)fb;
    while(1)
	{		
		// 通过对比数组中的汉字编码，定位字模地址
		if ( *(font->pTable + (fb_index+1)*font->Sizes + 0)==*ch && *(font->pTable + (fb_index+1)*font->Sizes + 1)==*(ch+1) )	
		{
		  char_index=fb_index;	// 字模地址偏移
		  break;
		}				
		fb_index+=2;	// 每个中文字符占两字节
		if(fb_index >= font->Table_Rows)	return;	// 字模列表没这个字
	}
    fb_index = 0;
    // 计算字模数据偏移量
    const uint8_t* char_data = font->pTable + char_index * font->Sizes;
    // 计算每行需要的字节数
    uint8_t bytes_per_row = (font->Width + 7) / 8;
    // 遍历每一行
    for (uint8_t row = 0; row < font->Height; row++) {
        for (uint8_t byte_idx = 0; byte_idx < bytes_per_row; byte_idx++) {
            uint8_t line_byte = char_data[row * bytes_per_row + byte_idx];
            uint8_t start_col = byte_idx * 8;

            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t col = start_col + bit;
                if (col >= font->Width) break;

                fb_index = (y + row) * g_akiegui.fb_width + (x + col);
                if (line_byte & (1 << (7 - bit))) {
                   fb32[fb_index] = color;
                } else if (!transparent) {
                    fb32[fb_index] = bg_color;
                }
            }
        }
    }
    #endif
}

/**
  * @brief	字符串绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 字符坐标 X
  *	@param	y: 字符坐标 Y
  * @param  str: 字符串
  *	@param	color: 字符颜色
  *	@param	bg_color: 背景颜色
  * @param  transparent: 背景是否透明
  * @param  font: 使用的字体
*/
void akiegui_draw_string(
    void *fb,
    uint16_t x, uint16_t y,
    const char *str,
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *font
) {
    if (!str || !font) return;
    
    uint16_t cur_x = x;
    while (*str) {
        akiegui_draw_char(fb, cur_x, y, *str, color, bg_color, transparent, font);
        cur_x += font->Width;
        str++;
    }
}

/**
  * @brief	字符串绘制
  *	@param	fb: 绘制缓冲区
  *	@param	x: 字符坐标 X
  *	@param	y: 字符坐标 Y
  * @param  str: 字符串
  *	@param	color: 字符颜色
  *	@param	bg_color: 背景颜色
  * @param  transparent: 背景是否透明
  * @param  chinese_font: 使用的中文字体
  * @param  ascii_font: 使用的英文字体
*/
void akiegui_draw_chinese_string(
    void *fb,
    uint16_t x, uint16_t y,
    const char *str,
    akiegui_color_t color,
    akiegui_color_t bg_color,
    uint8_t transparent,
    pFONT *chinese_font,
    pFONT *ascii_font
) {
    uint16_t cur_x = x;
    while (*str != '\0') {
        if ((uint8_t)*str >= 0xA1 && (uint8_t)*str <= 0xF7) {
            // GB2312 中文字符
            if (*(str + 1) == '\0') break;
            akiegui_draw_chinese_char(fb, cur_x, y, str, color, bg_color, transparent, chinese_font);
            cur_x += chinese_font->Width;
            str += 2;
        } else if (*str >= 0x20 && *str <= 0x7E) {
            // ASCII
            akiegui_draw_char(fb, cur_x, y, *str, color, bg_color, transparent, ascii_font);
            cur_x += ascii_font->Width;
            str++;
        } else {
            str++;
        }
    }
}

/* 计算字符串宽度 */
uint16_t akiegui_text_width(const char *str, pFONT *font) {
    if (!str || !font) return 0;
    return strlen(str) * font->Width;
}