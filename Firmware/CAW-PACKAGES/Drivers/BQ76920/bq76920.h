#ifndef __BQ76920_H__
#define __BQ76920_H__

#include <i2c.h>
#include <stdint.h>

#define BQ76920_I2C_ADDRESS (0x08 << 1)
#define BQ76920_I2C_TIMEOUT 500
#define BQ76920_GAIN_BASE 365
#define BQ76920_OV_TARGET 4.18
#define BQ76920_UV_TARGET 2.7

typedef struct _BQ76920_T {
  I2C_HandleTypeDef* hi2c;
  uint16_t GAIN;
  uint16_t OFFSET;
  float CellVoltage[5];
} BQ76920_T;

typedef enum _BQ76920_REG_MAP_E {
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
} BQ76920_REG_MAP_E;

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

typedef struct _BQ76920_CELLBAL1_T {
  uint8_t CB1;
  uint8_t CB2;
  uint8_t CB3;
  uint8_t CB4;
  uint8_t CB5;
} BQ76920_CELLBAL1_T;

/**
 * @description: 短路保护延迟
 * @return {*}
 */
typedef enum _BQ76920_SCD_DELAY_E {
  SCD_DELAY_70us = 0x0,
  SCD_DELAY_100us = 0x1,
  SCD_DELAY_200us = 0x2,
  SCD_DELAY_400us = 0x3,
} BQ76920_SCD_DELAY_E;

/**
 * @description: 短路保护阈值设置
 * 当PROTECT1(Bit 7) RSNS = 1时，阈值翻倍
 * @return {*}
 */
typedef enum _BQ76920_SCD_THRESHOLD_E {
  SCD_THRESHOLD_22mV = 0x0,
  SCD_THRESHOLD_33mV = 0x01,
  SCD_THRESHOLD_44mV = 0x02,
  SCD_THRESHOLD_56mV = 0x03,
  SCD_THRESHOLD_67mV = 0x04,
  SCD_THRESHOLD_78mV = 0x05,
  SCD_THRESHOLD_89mV = 0x06,
  SCD_THRESHOLD_100mV = 0x07,
} BQ76920_SCD_THRESHOLD_E;

/**
 * @description: 过流保护延迟
 * @return {*}
 */
typedef enum _BQ76920_OCD_DELAY_E {
  OCD_DELAY_8ms = 0x0,
  OCD_DELAY_20ms = 0x01,
  OCD_DELAY_40ms = 0x02,
  OCD_DELAY_80ms = 0x03,
  OCD_DELAY_160ms = 0x04,
  OCD_DELAY_320ms = 0x05,
  OCD_DELAY_640ms = 0x06,
  OCD_DELAY_1280ms = 0x07,
} BQ76920_OCD_DELAY_E;

/**
 * @description: 过流保护电压阈值枚举
 * 当PROTECT1(Bit 7) RSNS = 1时，阈值翻倍
 * @return {*}
 */
typedef enum _BQ76920_OCD_THRESHOLD_E {
  OCD_THRESHOLD_8mV = 0x0,
  OCD_THRESHOLD_11mV = 0x01,
  OCD_THRESHOLD_14mV = 0x02,
  OCD_THRESHOLD_17mV = 0x03,
  OCD_THRESHOLD_19mV = 0x04,
  OCD_THRESHOLD_22mV = 0x05,
  OCD_THRESHOLD_25mV = 0x06,
  OCD_THRESHOLD_28mV = 0x07,
  OCD_THRESHOLD_31mV = 0x08,
  OCD_THRESHOLD_33mV = 0x09,
  OCD_THRESHOLD_36mV = 0xA,
  OCD_THRESHOLD_39mV = 0xB,
  OCD_THRESHOLD_42mV = 0xC,
  OCD_THRESHOLD_44mV = 0xD,
  OCD_THRESHOLD_47mV = 0xE,
  OCD_THRESHOLD_50mV = 0xF,
} BQ76920_OCD_THRESHOLD_E;

/**
 * @description: 欠压保护延迟
 * @return {*}
 */
typedef enum _UV_DELAY_E {
  UV_DELAY_1s = 0x0,
  UV_DELAY_4s = 0x01,
  UV_DELAY_8s = 0x02,
  UV_DELAY_16s = 0x03,
} UV_DELAY_E;

/**
 * @description: 过压保护延迟
 * @return {*}
 */
typedef enum _OV_DELAY_E {
  OV_DELAY_1s = 0x0,
  OV_DELAY_2s = 0x01,
  OV_DELAY_4s = 0x02,
  OV_DELAY_8s = 0x03,
} OV_DELAY_E;

int BQ76920_Init(BQ76920_T* bq, I2C_HandleTypeDef* i2c);
int BQ76920_SysStat(BQ76920_T* bq, BQ76920_SYS_STAT_T* st);
int BQ76920_SysCtrl2(BQ76920_T* bq, BQ76920_SYS_CTRL2_T* st);
int BQ76920_CellBal1(BQ76920_T* bq, BQ76920_CELLBAL1_T* st);
int BQ76920_UpdateCellVoltage(BQ76920_T* bq);
int BQ76920_Shutdown(BQ76920_T* bq);

#endif