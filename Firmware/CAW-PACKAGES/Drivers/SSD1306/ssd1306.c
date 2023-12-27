/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-11-27 23:35:20
 * @LastEditors: Rick
 * @LastEditTime: 2023-12-26 16:05:03
 * @FilePath: \CAW-Robot\Core\Drivers\SSD1306\ssd1306.c
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#include "ssd1306.h"

#include <stdarg.h>
#include <string.h>

#include "fonts.h"

SSD1306_T G_SSD1306_Instance = {NULL, false, false, false, 0, 0, 0};

static uint8_t G_SSD1306_BUFFER[8][128] = {0};

#define SSD1306_SPI_DC_PORT OLED_DC_GPIO_Port
#define SSD1306_SPI_DC_PIN OLED_DC_Pin
#define SSD1306_SPI_CS_PORT SPI1_NSS_GPIO_Port
#define SSD1306_SPI_CS_PIN SPI1_NSS_Pin

#define SET_SPI_CTL() \
  HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)

#define SET_SPI_DATA() \
  HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)

#define SET_SPI_CS(sta) \
  HAL_GPIO_WritePin(SSD1306_SPI_CS_PORT, SSD1306_SPI_CS_PIN, sta)

/**
 * @description: 加锁
 * @return {*}
 */
void SSD1306_Lock() {
  while (G_SSD1306_Instance.mutex)
    ;
  G_SSD1306_Instance.mutex = 1;
}

/**
 * @description: 解锁
 * @return {*}
 */
void SSD1306_Unlock() { G_SSD1306_Instance.mutex = 0; }

/**
 * @description: SSD1306写字节函数
 * @param {uint8_t} ctl_byte 控制字节：SSD1306_CMD或SSD1306_DATA
 * @param {uint8_t} data 数据
 * @return {*} 处理状态
 */
#ifndef SSD1306_USE_SPI
HAL_StatusTypeDef _SSD1306_WriteData(uint8_t ctl_byte, uint8_t* data,
                                     uint16_t size) {
  HAL_StatusTypeDef sta = HAL_ERROR;
  if (!G_SSD1306_Instance.initialized) return sta;
  if (G_SSD1306_Instance.enable_dma) {
    SSD1306_Lock();
    while (HAL_I2C_GetState(G_SSD1306_Instance.i2c_ins) != HAL_I2C_STATE_READY)
      ;
    sta = HAL_I2C_Mem_Write_DMA(G_SSD1306_Instance.i2c_ins, SSD1306_ADDR,
                                ctl_byte, I2C_MEMADD_SIZE_8BIT, data, size);
    SSD1306_Unlock();
  } else {
    sta = HAL_I2C_Mem_Write(G_SSD1306_Instance.i2c_ins, SSD1306_ADDR, ctl_byte,
                            I2C_MEMADD_SIZE_8BIT, data, size, -1);
  }
  return sta;
}
#else
HAL_StatusTypeDef _SSD1306_WriteData(uint8_t ctl_byte, uint8_t* data,
                                     uint16_t size) {
  HAL_StatusTypeDef sta = HAL_ERROR;
  if (!G_SSD1306_Instance.initialized) return sta;
  SET_SPI_CS(0);
  if (SSD1306_CMD == ctl_byte)
    SET_SPI_CTL();
  else if (SSD1306_DATA == ctl_byte)
    SET_SPI_DATA();

  if (G_SSD1306_Instance.enable_dma) {
    SSD1306_Lock();
    while (HAL_SPI_GetState(G_SSD1306_Instance.spi_ins) != HAL_SPI_STATE_READY)
      ;
    sta = HAL_SPI_Transmit_DMA(G_SSD1306_Instance.spi_ins, data, size);
    SSD1306_Unlock();
  } else {
    sta = HAL_SPI_Transmit(G_SSD1306_Instance.spi_ins, data, size, -1);
  }
  SET_SPI_CS(1);
  SET_SPI_DATA();
  return sta;
}
#endif

HAL_StatusTypeDef _SSD1306_WriteByte(uint8_t ctl_byte, uint8_t data) {
  return _SSD1306_WriteData(ctl_byte, &data, 1);
}

/**
 * @description: 写入控制数据
 * @return {*}
 */
#define SSD1306_WriteCMD(data) _SSD1306_WriteByte(SSD1306_CMD, data)

/**
 * @description: 写入屏幕数据(DMA形式)
 * @return {*}
 */
#define SSD1306_WriteData(data_ptr, size) \
  if (G_SSD1306_Instance.enable_dma)      \
  _SSD1306_WriteData(SSD1306_DATA, data_ptr, size)

/**
 * @description: 写入一个字节
 * @return {*}
 */
#define SSD1306_WriteByte(data) _SSD1306_WriteByte(SSD1306_DATA, data)

/**
 * @description: SSD1306初始化函数
 * @param {I2C_HandleTypeDef} *hi2c i2c对象
 * @return {*} 处理状态
 */
int SSD1306_Init(I2C_HandleTypeDef* hi2c, SPI_HandleTypeDef* hspi,
                 bool enable_dma) {
  if (G_SSD1306_Instance.initialized) return -1;
  G_SSD1306_Instance.initialized = true;
  G_SSD1306_Instance.i2c_ins = hi2c;
  G_SSD1306_Instance.spi_ins = hspi;
  G_SSD1306_Instance.enable_dma = enable_dma;
  G_SSD1306_Instance.mutex = 0;

  SSD1306_WriteCMD(0xae);  // 关闭显示
  SSD1306_WriteCMD(0x20);  // 设置内存寻址模式Set Memory Addressing Mode
  // 00：水平寻址模式 01：垂直寻址模式 02：页面寻址模式(复位)
  SSD1306_WriteCMD(0x00);
  SSD1306_WriteCMD(0x81);  // 设置对比度
  SSD1306_WriteCMD(0xff);  // 对比度,数值越大对比度越高
  SSD1306_WriteCMD(0xc8);  // 扫描方向 不上下翻转
  SSD1306_WriteCMD(0xa1);  // 设置段重新映射 不左右翻转
  SSD1306_WriteCMD(0xa8);  // 设置多路复用比(1-64)
  SSD1306_WriteCMD(0x3f);  // 设定值1/32  1/32
  SSD1306_WriteCMD(0xd3);  // 设置显示偏移
  SSD1306_WriteCMD(0x00);  //
  SSD1306_WriteCMD(0xd5);  // 设置osc分区
  SSD1306_WriteCMD(0x80);  //
  SSD1306_WriteCMD(0xd8);  // 关闭区域颜色模式
  SSD1306_WriteCMD(0x05);  //
  SSD1306_WriteCMD(0xd9);  // 设置预充电期
  SSD1306_WriteCMD(0xf1);  //
  SSD1306_WriteCMD(0xda);  // 设置com引脚配置
  SSD1306_WriteCMD(0x12);  //
  SSD1306_WriteCMD(0xdb);  // 设置vcomh set Vcomh
  SSD1306_WriteCMD(0x30);  //
  SSD1306_WriteCMD(0x8d);  // 设置电源泵启用
  SSD1306_WriteCMD(0x14);  //
  SSD1306_WriteCMD(0xa4);  // 设置全局显示
  SSD1306_WriteCMD(0xa6);  // 正常显示

  SSD1306_SetPage(0, SSD1306_PAGE_NUM - 1);
  SSD1306_SetColumn(0, SSD1306_WIDTH - 1);

  SSD1306_Fill(0x00);  // 清屏
  SSD1306_Update();
  SSD1306_WriteCMD(0xaf);  // 显示
  return 0;
}

/**
 * @description: 设置坐标
 * @param {uint8_t} x
 * @param {uint8_t} y
 * @return {*}
 */
void SSD1306_SetPos(uint8_t x, uint8_t y) {
  G_SSD1306_Instance.x = x;
  G_SSD1306_Instance.y = y;

  uint8_t page = y / SSD1306_ROW_NUM;
  SSD1306_SetPage(page, page);
  SSD1306_SetColumn(x, x);
}

/**
 * @description:  设置页
 * @param {uint8_t} start 开始页
 * @param {uint8_t} end 结束页
 * @return {*}
 */
void SSD1306_SetPage(uint8_t start, uint8_t end) {
  SSD1306_WriteCMD(0x22);   // 设置页地址
  SSD1306_WriteCMD(start);  // 起始
  SSD1306_WriteCMD(end);    // 结束
}

/**
 * @description: 设置列
 * @param {uint8_t} start 开始列
 * @param {uint8_t} end 结束列
 * @return {*}
 */
void SSD1306_SetColumn(uint8_t start, uint8_t end) {
  SSD1306_WriteCMD(0x21);   // 设置列地址
  SSD1306_WriteCMD(start);  // 起始
  SSD1306_WriteCMD(end);    // 结束
}

/**
 * @description:  填充数据
 * @param {uint8_t} data 数据
 * @return {*}
 */
void SSD1306_Fill(uint8_t data) {
  if (G_SSD1306_Instance.filled && !G_SSD1306_Instance.enable_dma) {
    for (uint32_t i = 0; i < SSD1306_PAGE_NUM; i++) {
      for (uint32_t j = 0; j < SSD1306_WIDTH; j++) {
        if (G_SSD1306_BUFFER[i][j] != data) {
          G_SSD1306_BUFFER[i][j] = data;
          SSD1306_SetPage(i, i);
          SSD1306_SetColumn(j, j);
          SSD1306_WriteByte(data);
        }
      }
    }
  } else {
    for (uint32_t i = 0; i < SSD1306_PAGE_NUM; i++) {
      for (uint32_t j = 0; j < SSD1306_WIDTH; j++) {
        G_SSD1306_BUFFER[i][j] = data;
      }
    }
    if (!G_SSD1306_Instance.enable_dma) {
      G_SSD1306_Instance.filled = true;
      SSD1306_SetPage(0, SSD1306_PAGE_NUM - 1);
      SSD1306_SetColumn(0, SSD1306_WIDTH - 1);
      _SSD1306_WriteData(SSD1306_DATA, (uint8_t*)G_SSD1306_BUFFER,
                         sizeof(G_SSD1306_BUFFER));
    }
  }
}

/**
 * @description: 绘制一个点
 * @param {uint32_t} x 列（0-127）
 * @param {uint32_t} y 行 （0-63）
 * @param {SSD1306_COLOR} color （WHITE 或 BLACK）
 * @return {*}
 */
int SSD1306_DrawPoint(uint8_t x, uint8_t y, SSD1306_COLOR color) {
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return -1;
  // [y / 8]，计算出存放数据的页码，[y / 8][x]通过页码和列定位到存放该像素值
  // 的具体数据（8bit）。定位到数据后需要需要通过位计算控制像素点的亮灭。
  // BLACK需要将该位清0，可以通过与运算实现清零，WHITE需要将该位设置成1，
  // 可以通过或运算实现。y % 8求余后可以获取像素点在该页中的哪一位。
  if (color == BLACK) {
    G_SSD1306_BUFFER[y / 8][x] &= ~(1 << (y % 8));
  } else if (color == WHITE) {
    G_SSD1306_BUFFER[y / 8][x] |= 1 << (y % 8);
  }
  if (!G_SSD1306_Instance.enable_dma) {
    SSD1306_SetPos(x, y);
    SSD1306_WriteByte(G_SSD1306_BUFFER[y / 8][x]);
  }
  return 0;
}

/**
 * @description: 绘制一个字符
 * @param {char} c 字符
 * @param {uint8_t} x 列
 * @param {uint8_t} y 行
 * @param {SSD1306_FONT} font 字体
 * @return {*}
 */
int SSD1306_WriteChar(char c, uint8_t x, uint8_t y, SSD1306_FONT font) {
  uint8_t index = (uint8_t)(c - 0x20);
  if (font == FONT_6x8) {
    if (x >= SSD1306_WIDTH - 6 || y >= SSD1306_HEIGHT) return -1;
    for (int i = 0; i < 6; i++) {
      uint8_t d = *(ssd1306xled_font6x8 + index * 6 + i + 4);
      if (y % SSD1306_ROW_NUM == 0) {
        // 如果行是页的整倍数时
        G_SSD1306_BUFFER[y / SSD1306_ROW_NUM][x] = d;
        if (!G_SSD1306_Instance.enable_dma) {
          SSD1306_SetPos(x, y);
          SSD1306_WriteByte(G_SSD1306_BUFFER[y / SSD1306_ROW_NUM][x]);
        }

      } else {
        if (y / SSD1306_ROW_NUM + 1 >= SSD1306_PAGE_NUM) return -1;
        uint8_t page = y / SSD1306_ROW_NUM;
        int p1 = 8 - y % SSD1306_ROW_NUM;
        int p2 = y % SSD1306_ROW_NUM;
        G_SSD1306_BUFFER[page][x] =
            (d << p2) | ((G_SSD1306_BUFFER[page][x] << p1) >> p1);
        if (!G_SSD1306_Instance.enable_dma) {
          SSD1306_SetPos(x, y);
          SSD1306_WriteByte(G_SSD1306_BUFFER[page][x]);
        }

        page++;
        G_SSD1306_BUFFER[page][x] =
            (d >> p1) | ((G_SSD1306_BUFFER[page][x] >> p2) << p2);
        if (!G_SSD1306_Instance.enable_dma) {
          SSD1306_SetPage(page, page);
          SSD1306_WriteByte(G_SSD1306_BUFFER[page][x]);
        }
      }
      if (x + 1 >= SSD1306_WIDTH) return -1;
      ++x;
    }
  }
  return 0;
}

/**
 * @description: 绘制字符串
 * @param {char*} str 字符串
 * @param {uint8_t} x 列
 * @param {uint8_t} y 行
 * @param {SSD1306_FONT} font 字体
 * @return {*}
 */
int SSD1306_WriteString(const char* str, uint8_t x, uint8_t y,
                        SSD1306_FONT font) {
  int sta = -1;
  SSD1306_Lock();
  uint32_t step = 0;
  if (font == FONT_6x8) {
    step = 6;
  } else {
    goto _return;
  }
  for (uint32_t i = 0; i < strlen(str); i++) {
    if (SSD1306_WriteChar(str[i], x, y, font) < 0) {
      goto _return;
    }
    x += step;
  }
  sta = 0;
_return:
  SSD1306_Unlock();
  return sta;
}

/**
 * @description: 刷新缓冲区的内容到屏幕（给DMA模式使用）
 * @return {*}
 */
void SSD1306_Update() {
  if (G_SSD1306_Instance.enable_dma) {
    SSD1306_WriteData((uint8_t*)G_SSD1306_BUFFER, sizeof(G_SSD1306_BUFFER));
    HAL_Delay(0);
  }
}

/**
 * @description: 格式化打印字符串
 * @param {uint8_t} x 列
 * @param {uint8_t} y 行
 * @param {SSD1306_FONT} font 字体
 * @param {char*} fmt 格式化字符串
 * @return {*}
 */
int SSD1306_Printf(uint8_t x, uint8_t y, SSD1306_FONT font, const char* fmt,
                   ...) {
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), (char*)fmt, args);
  va_end(args);
  SSD1306_WriteString(buf, x, y, font);
  return 0;
}
