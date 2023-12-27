/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-12-06 01:35:07
 * @LastEditors: Rick rick@guaik.io
 * @LastEditTime: 2022-12-06 16:11:42
 * @FilePath: \CAW-Robot\Core\Drivers\SSD1306\fonts.h
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t ssd1306xled_font6x8[];

extern const uint8_t ssd1306xled_font8x16[];

extern const uint8_t ssd1306xled_font6x8_German[];

extern const uint8_t ssd1306xled_font6x8_AB[];

extern const uint8_t ssd1306xled_font5x7[];

extern const uint8_t ssd1306xled_font5x7_AB[];

extern const uint8_t digital_font5x7_123[];

extern const uint8_t digital_font5x7_AB[];

extern const uint8_t digital_font5x7[];

extern const uint8_t free_calibri11x12[];

extern const uint8_t free_calibri11x12_cyrillic[];

extern const uint8_t free_calibri11x12_latin[];

extern const uint8_t courier_new_font11x16_digits[];

extern const uint8_t comic_sans_font24x32_123[];

#ifdef __cplusplus
}
#endif

#endif  // SSD1306_FONTS_H