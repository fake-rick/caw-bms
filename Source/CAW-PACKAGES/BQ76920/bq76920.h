#ifndef __BQ76920_H__
#define __BQ76920_H__

#include <i2c.h>
#include <stdint.h>

#define BQ76920_I2C_ADDRESS (0x08 << 1)
#define BQ76920_I2C_TIMEOUT 500

typedef struct _BQ76920_T {
  I2C_HandleTypeDef* hi2c;
} BQ76920_T;

typedef enum _BQ76920_REG_MAP {
  SYS_STAT = 0x00,
  CELLBAL1 = 0x01,
  CELLBAL2 = 0x02,
  CELLBAL3 = 0x03,
  SYS_CTRL1 = 0x04,
  SYS_CTRL2 = 0x05,
  PROTECT1 = 0x06,
  PROTECT2 = 0x07,
  PROTECT3 = 0x08,
  OV_TRIP = 0x09,
  UV_TRIP = 0x0A,
  CC_CFG = 0x0B,
  VC1_HI = 0x0C,
  VC1_LO = 0x0D,
  VC2_HI = 0x0E,
  VC2_LO = 0x0F,
  VC3_HI = 0x10,
  VC3_LO = 0x11,
  VC4_HI = 0x12,
  VC4_LO = 0x13,
  VC5_HI = 0x14,
  VC5_LO = 0x15,
  BAT_HI = 0x2A,
  BAT_LO = 0x2B,
  TS1_HI = 0x2C,
  TS1_LO = 0x2D,
  CC_HI = 0x32,
  CC_LO = 0x33,
  ADCGAIN1 = 0x50,
  ADCOFFSET = 0x51,
  ADCGAIN2 = 0x59,
} BQ76920_REG_MAP;

typedef struct _BQ76920_SYS_STAT_T {
  uint8_t OCD;
  uint8_t SCD;
  uint8_t OV;
  uint8_t UV;
  uint8_t OVRD_ALERT;
  uint8_t DEVICE_XREADY;
  uint8_t RSVD;
  uint8_t CC_READY;
} BQ76920_SYS_STAT_T;

typedef struct _BQ76920_SYS_CTRL2_T {
  uint8_t CHG_ON;
  uint8_t DSG_ON;
  uint8_t RSVD_0;
  uint8_t RSVD_1;
  uint8_t RSVD_2;
  uint8_t CC_ONESHOT;
  uint8_t CC_EN;
  uint8_t DELAY_DIS;
} BQ76920_SYS_CTRL2_T;

int BQ76920_Init(BQ76920_T* bq, I2C_HandleTypeDef* i2c);
int BQ76920_SysStat(BQ76920_T* bq, BQ76920_SYS_STAT_T* st);
int BQ76920_SysCtrl2(BQ76920_T* bq, BQ76920_SYS_CTRL2_T* st);

#endif