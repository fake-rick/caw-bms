#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include <stdint.h>

#define PROTOCOL_BUF_SIZE 512

#pragma pack(1)
typedef struct _PACK_HEAD {
  uint8_t magic[4];
  uint16_t cmd;
  uint32_t length;
  uint16_t version;
} PACK_HEAD;
#pragma pack()

typedef struct _PROTOCOL_T {
  uint8_t recv_buf[PROTOCOL_BUF_SIZE];
} PROTOCOL_T;

#endif