/*
 * @Author: Rick rick@guaik.io
 * @Date: 2023-07-03 00:56:04
 * @LastEditors: Rick
 * @LastEditTime: 2023-08-13 14:58:50
 * @Description:
 */

#ifndef __ENDIAN_H__
#define __ENDIAN_H__
#include <stdint.h>

typedef union {
  float f;
  char c[4];
} FLOAT_CONV_T;

float EndianFloat(float v);

uint16_t EndianUInt16(uint16_t v);
uint32_t EndianUInt32(uint32_t v);
uint64_t EndianUint64(uint64_t v);

#endif