#include "bq76920.h"

#include "caw_status.h"
#include "log.h"

uint8_t _crc8_ccitt_update(uint8_t inCrc, uint8_t inData) {
  uint8_t i;
  uint8_t data;
  data = inCrc ^ inData;
  for (i = 0; i < 8; i++) {
    if ((data & 0x80) != 0) {
      data <<= 1;
      data ^= 0x07;
    } else {
      data <<= 1;
    }
  }
  return data;
}

int _I2C_READ_BYTE(BQ76920_T* bq, uint16_t reg, uint8_t* data) {
  if (HAL_OK != HAL_I2C_Mem_Read(bq->hi2c, BQ76920_I2C_ADDRESS, reg,
                                 I2C_MEMADD_SIZE_8BIT, data, 1,
                                 BQ76920_I2C_TIMEOUT)) {
    CAW_LOG_ERROR("_I2C_READ_BYTE failed");
    return CAW_ERR;
  }
  return CAW_OK;
}

int _I2C_WRITE_BYTE(BQ76920_T* bq, uint16_t reg, uint8_t data) {
  uint8_t buf[3];
  uint8_t crc = 0;
  buf[0] = reg;
  buf[1] = data;
  crc = _crc8_ccitt_update(crc, BQ76920_I2C_ADDRESS | 0);
  crc = _crc8_ccitt_update(crc, buf[0]);
  crc = _crc8_ccitt_update(crc, buf[1]);
  buf[2] = crc;
  HAL_I2C_Master_Transmit(bq->hi2c, BQ76920_I2C_ADDRESS, buf, sizeof(buf),
                          BQ76920_I2C_TIMEOUT);
  return CAW_OK;
}

int BQ76920_Init(BQ76920_T* bq, I2C_HandleTypeDef* i2c) {
  bq->hi2c = i2c;

  _I2C_WRITE_BYTE(bq, SYS_STAT, 0xff);

  _I2C_WRITE_BYTE(bq, CC_CFG, 0x19);

  _I2C_WRITE_BYTE(bq, SYS_CTRL1, 0x10);

  _I2C_WRITE_BYTE(bq, SYS_CTRL2, 0x43);

  return 0;
}

int BQ76920_SysStat(BQ76920_T* bq, BQ76920_SYS_STAT_T* st) {
  uint8_t data;
  if (CAW_OK != _I2C_READ_BYTE(bq, SYS_STAT, &data)) {
    return CAW_ERR;
  }
  st->OCD = data & 0x01;
  st->SCD = (data >> 1) & 0x01;
  st->OV = (data >> 2) & 0x01;
  st->UV = (data >> 3) & 0x01;
  st->OVRD_ALERT = (data >> 4) & 0x01;
  st->DEVICE_XREADY = (data >> 5) & 0x01;
  st->RSVD = (data >> 6) & 0x01;
  st->CC_READY = (data >> 7) & 0x01;
  return CAW_OK;
}

int BQ76920_SysCtrl2(BQ76920_T* bq, BQ76920_SYS_CTRL2_T* st) {
  uint8_t data;
  if (CAW_OK != _I2C_READ_BYTE(bq, SYS_CTRL2, &data)) {
    return CAW_ERR;
  }
  CAW_LOG_DEBUG("DATA %x", data);
  st->CHG_ON = data & 0x01;
  st->DSG_ON = (data >> 1) & 0x01;
  st->RSVD_0 = (data >> 2) & 0x01;
  st->RSVD_1 = (data >> 3) & 0x01;
  st->RSVD_2 = (data >> 4) & 0x01;
  st->CC_ONESHOT = (data >> 5) & 0x01;
  st->CC_EN = (data >> 6) & 0x01;
  st->DELAY_DIS = (data >> 7) & 0x01;
  return CAW_OK;
}