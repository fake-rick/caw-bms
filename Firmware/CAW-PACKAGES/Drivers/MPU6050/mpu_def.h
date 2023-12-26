/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-12-10 18:33:17
 * @LastEditors: Rick rick@guaik.io
 * @LastEditTime: 2022-12-13 20:02:27
 * @FilePath: \CAW-Robot\Core\Drivers\MPU6050\mpu_def.h
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#ifndef __FUNCS_H__
#define __FUNCS_H__

#include <i2c.h>
#include <math.h>

typedef struct {
  I2C_HandleTypeDef *i2c_ins;
} MPU_DEF;

extern MPU_DEF G_MPU_DEF;

#define i2c_write(a, b, c, d) \
  HAL_I2C_Mem_Write(G_MPU_DEF.i2c_ins, a, b, I2C_MEMADD_SIZE_8BIT, d, c, -1)
#define i2c_read(a, b, c, d) \
  HAL_I2C_Mem_Read(G_MPU_DEF.i2c_ins, a, b, I2C_MEMADD_SIZE_8BIT, d, c, -1)
#define delay_ms(n) HAL_Delay(n)
#define get_ms(p)       \
  do {                  \
    *p = HAL_GetTick(); \
  } while (0)

#define labs abs
#define fabs(x) (((x) >= 0) ? (x) : -(x))
#define min(x, y) ((x < y) ? x : y)

#define log_i(...) \
  do {             \
  } while (0)

#define log_e(...) \
  do {             \
  } while (0)
#endif