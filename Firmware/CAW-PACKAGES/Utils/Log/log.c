/*
 * @Author: Rick rick@guaik.io
 * @Date: 2023-08-29 23:50:14
 * @LastEditors: Rick
 * @LastEditTime: 2023-09-25 21:18:16
 * @Description:
 */

#include "log.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"

/**
 * @description: 日志发送函数，在该函数中实现发送逻辑
 * @param {uint8_t*} buf
 * @param {uint32_t} size
 * @return {*}
 */
inline void _Write(const uint8_t *buf, uint32_t size) {
  HAL_UART_Transmit(&huart1, buf, size, HAL_MAX_DELAY);
}

void CAW_LOG_Write(const char *fmt, CAW_LOG_LEVEL level, const char *file,
                   int line, const char *func, ...) {
  char tmp[128];
  char buf[128];
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  va_list args;
  va_start(args, fmt);
  vsprintf(tmp, (char *)fmt, args);
  va_end(args);

  if (level == LOG_LEVEL_DEBUG) {
#ifdef CAWFOC_LOG_COLOR_ENABLE
    snprintf(buf, sizeof(buf),
             "\033[0;36mCawBMS-[DEBUG] <%s> | <%d> | <%s>: %s\033[m\r\n", file,
             line, func, tmp);
#else
    snprintf(buf, sizeof(buf), "CawFOC-[DEBUG] <%s> | <%d> | <%s>: %s\r\n",
             file, line, func, tmp);
#endif
  } else if (level == LOG_LEVEL_INFO) {
#ifdef CAWFOC_LOG_COLOR_ENABLE
    snprintf(buf, sizeof(buf),
             "\033[0;32mCawBMS-[INFO] <%s> | <%d> | <%s>: %s\033[m\r\n", file,
             line, func, tmp);
#else
    snprintf(buf, sizeof(buf), "CawFOC-[INFO] <%s> | <%d> | <%s>: %s\r\n", file,
             line, func, tmp);
#endif
  } else if (level == LOG_LEVEL_WARN) {
#ifdef CAWFOC_LOG_COLOR_ENABLE
    snprintf(buf, sizeof(buf),
             "\033[0;33mCawBMS-[WARN] <%s> | <%d> | <%s>: %s\033[m\r\n", file,
             line, func, tmp);
#else
    snprintf(buf, sizeof(buf), "CawFOC-[WARN] <%s> | <%d> | <%s>: %s\r\n", file,
             line, func, tmp);
#endif
  } else if (level == LOG_LEVEL_ERROR) {
#ifdef CAWFOC_LOG_COLOR_ENABLE
    snprintf(buf, sizeof(buf),
             "\033[0;31mCawBMS-[ERROR] <%s> | <%d> | <%s>: %s\033[m\r\n", file,
             line, func, tmp);
#else
    snprintf(buf, sizeof(buf), "CawFOC-[ERROR] <%s> | <%d> | <%s>: %s\r\n",
             file, line, func, tmp);
#endif
  } else if (level == LOG_LEVEL_FAIL) {
    while (1) {
#ifdef CAWFOC_LOG_COLOR_ENABLE
      snprintf(buf, sizeof(buf),
               "\033[0;35mCawBMS-[FAIL] <%s> | <%d> | <%s>: %s\033[m\r\n", file,
               line, func, tmp);
#else
      snprintf(buf, sizeof(buf), "CawFOC-[ERROR] <%s> | <%d> | <%s>: %s\r\n",
               file, line, func, tmp);
#endif
      HAL_Delay(1000);
    }
  } else {
    return;
  }
  _Write(buf, strlen(buf) + 1);
}

void CAW_LOG_RAW(const char *str) { _Write(str, strlen(str) + 1); }

void CAW_LOG_Welcome(void) {
  char buf[] =
      "\r\n"
      "|----------------------------------------|\r\n"
      "|            \033[0;31mWelcome to CawBMS\033[m           |\r\n"
      "|----------------------------------------|\r\n"
      "|            Author: FakeRick            |\r\n"
      "|----------------------------------------|\r\n"
      "|              MIT License               |\r\n"
      "|----------------------------------------|\r\n"
      "|  https://space.bilibili.com/358330282  |\r\n"
      "\r\n";
  _Write(buf, strlen(buf) + 1);
}