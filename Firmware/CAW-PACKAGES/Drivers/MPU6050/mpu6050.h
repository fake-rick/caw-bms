/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-12-10 00:11:34
 * @LastEditors: Rick rick@guaik.io
 * @LastEditTime: 2023-01-09 15:38:46
 * @FilePath: \SSD1306\CAW-PACKAGES\Drivers\MPU6050\mpu6050.h
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#ifndef __MPU6050_H__
#define __MPU6050_H__
#include <i2c.h>
#include <stdbool.h>

typedef struct {
  I2C_HandleTypeDef *i2c_ins;
  bool enable_dma;
  bool initialized;
} MPU6050_T;

typedef struct {
  float pitch;
  float roll;
  float yaw;
} MPU6050_DATA_T;

int MPU6050_Init(I2C_HandleTypeDef *hi2c, bool enable_dma);
int MPU6050_GetData(float *pitch, float *roll, float *yaw);

#endif