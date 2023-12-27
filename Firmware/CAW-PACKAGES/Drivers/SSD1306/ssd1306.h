/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-11-27 23:35:15
 * @LastEditors: Rick rick@guaik.io
 * @LastEditTime: 2022-12-10 00:00:16
 * @FilePath: \CAW-Robot\Core\Drivers\SSD1306\ssd1306.h
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#ifndef __SSD1306_H__
#define __SSD1306_H__
#include <stdbool.h>
#include <stdint.h>

#include "i2c.h"
#include "spi.h"

#define SSD1306_USE_SPI 1

#define SSD1306_ADDR 0x78
#define SSD1306_CMD 0x00
#define SSD1306_DATA 0x40
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_ROW_NUM 8   // 每页行数
#define SSD1306_PAGE_NUM 8  // 总页数

typedef struct {
  I2C_HandleTypeDef *i2c_ins;
  SPI_HandleTypeDef *spi_ins;
  bool enable_dma;
  bool initialized;
  bool filled;
  uint8_t mutex;
  uint8_t x;
  uint8_t y;
} SSD1306_T;

typedef enum {
  BLACK = 0,
  WHITE = 1,
} SSD1306_COLOR;

typedef enum { FONT_6x8 = 0, FONT_6X16 = 1 } SSD1306_FONT;

extern SSD1306_T G_SSD1306_Instance;

int SSD1306_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi,
                 bool enable_dma);
void SSD1306_Fill(uint8_t data);
int SSD1306_DrawPoint(uint8_t x, uint8_t y, SSD1306_COLOR color);
void SSD1306_SetPos(uint8_t x, uint8_t y);
void SSD1306_SetPage(uint8_t start, uint8_t end);
void SSD1306_SetColumn(uint8_t start, uint8_t end);
int SSD1306_WriteChar(char c, uint8_t x, uint8_t y, SSD1306_FONT font);
int SSD1306_WriteString(const char *str, uint8_t x, uint8_t y,
                        SSD1306_FONT font);
int SSD1306_Printf(uint8_t x, uint8_t y, SSD1306_FONT font, const char *fmt,
                   ...);
void SSD1306_Update();
#endif