/*
 * @Author: Rick rick@guaik.io
 * @Date: 2023-08-29 23:50:14
 * @LastEditors: Rick
 * @LastEditTime: 2023-09-01 16:46:56
 * @Description:
 */

#ifndef __LOG_H__
#define __LOG_H__
#include <stdarg.h>
#include <stdbool.h>
#include <usart.h>

typedef enum {
  LOG_LEVEL_DEBUG = 0x1000,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FAIL,
} CAW_LOG_LEVEL;

/******************************************************************/
// 设置日志级别
#define CAWFOC_LOG_LEVEL LOG_LEVEL_INFO
// 启动用日志输出控制台颜色
#define CAWFOC_LOG_COLOR_ENABLE
/******************************************************************/

void CAW_LOG_Welcome(void);

void CAW_LOG_Write(const char* fmt, CAW_LOG_LEVEL level, const char* file,
                   int line, const char* func, ...);
void CAW_LOG_RAW(const char* str);

#if CAWFOC_LOG_LEVEL == LOG_LEVEL_DEBUG
#define CAW_LOG_DEBUG(fmt, ...)                                         \
  CAW_LOG_Write(fmt, LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, \
                ##__VA_ARGS__)
#else
#define CAW_LOG_DEBUG(fmt, ...) \
  {}
#endif

#if CAWFOC_LOG_LEVEL <= LOG_LEVEL_INFO
#define CAW_LOG_INFO(fmt, ...)                                         \
  CAW_LOG_Write(fmt, LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, \
                ##__VA_ARGS__)
#else
#define CAW_LOG_INFO(fmt, ...) \
  {}
#endif

#if CAWFOC_LOG_LEVEL <= LOG_LEVEL_WARN
#define CAW_LOG_WARN(fmt, ...)                                         \
  CAW_LOG_Write(fmt, LOG_LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, \
                ##__VA_ARGS__)
#else
#define CAW_LOG_WARN(fmt, ...) \
  {}
#endif

#if CAWFOC_LOG_LEVEL <= LOG_LEVEL_ERROR
#define CAW_LOG_ERROR(fmt, ...)                                         \
  CAW_LOG_Write(fmt, LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, \
                ##__VA_ARGS__)
#else
#define CAW_LOG_ERROR(fmt, ...) \
  {}
#endif

#if CAWFOC_LOG_LEVEL <= LOG_LEVEL_FAIL
#define CAW_LOG_FAIL(fmt, ...)                                         \
  CAW_LOG_Write(fmt, LOG_LEVEL_FAIL, __FILE__, __LINE__, __FUNCTION__, \
                ##__VA_ARGS__)
#else
#define CAW_LOG_ERROR(fmt, ...) \
  {}
#endif

#endif