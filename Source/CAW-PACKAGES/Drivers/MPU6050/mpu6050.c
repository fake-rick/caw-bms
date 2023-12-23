/*
 * @Author: Rick rick@guaik.io
 * @Date: 2022-12-10 00:11:40
 * @LastEditors: Rick rick@guaik.io
 * @LastEditTime: 2023-01-06 21:48:10
 * @FilePath: \MPU6050\CAW-PACKAGES\Drivers\MPU6050\mpu6050.c
 * @Description:
 * Copyright (c) 2022 by Rick email: rick@guaik.io, All Rights Reserved.
 */
#include "mpu6050.h"

#include "dmpKey.h"
#include "dmpmap.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "log.h"

#define DEFAULT_MPU_HZ 200
#define q30 1073741824.0f

MPU6050_T G_MPU6050_Instance = {NULL, false, false};

static signed char gyro_orientation[9] = {-1, 0, 0, 0, -1, 0, 0, 0, 1};

static inline unsigned short inv_row_2_scale(const signed char* row) {
  unsigned short b;

  if (row[0] > 0)
    b = 0;
  else if (row[0] < 0)
    b = 4;
  else if (row[1] > 0)
    b = 1;
  else if (row[1] < 0)
    b = 5;
  else if (row[2] > 0)
    b = 2;
  else if (row[2] < 0)
    b = 6;
  else
    b = 7;  // error
  return b;
}

static inline unsigned short inv_orientation_matrix_to_scalar(
    const signed char* mtx) {
  unsigned short scalar;

  /*
     XYZ  010_001_000 Identity Matrix
     XZY  001_010_000
     YXZ  010_000_001
     YZX  000_010_001
     ZXY  001_000_010
     ZYX  000_001_010
   */

  scalar = inv_row_2_scale(mtx);
  scalar |= inv_row_2_scale(mtx + 3) << 3;
  scalar |= inv_row_2_scale(mtx + 6) << 6;

  return scalar;
}

/**
 * @description: MPU6050初始化函数
 * @param {I2C_HandleTypeDef*} hi2c i2实例
 * @param {bool} enable_dma 是否启用DMA
 * @return {*} 0: 成功
 */
int MPU6050_Init(I2C_HandleTypeDef* hi2c, bool enable_dma) {
  int sta = 0;
  G_MPU6050_Instance.i2c_ins = hi2c;
  G_MPU6050_Instance.enable_dma = enable_dma;

  // struct int_param_s* p = 0;
  while (mpu_init(hi2c, NULL) != 0)
    ;
  if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
    CAW_LOG_ERROR("mpu_set_sensors failed");
    return -1;
  }
  if (mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
    CAW_LOG_ERROR("mpu_configure_fifo failed");
    return -1;
  }
  if (mpu_set_sample_rate(DEFAULT_MPU_HZ)) {
    CAW_LOG_ERROR("mpu_set_sample_rate failed");
    return -1;
  }
  if (dmp_load_motion_driver_firmware()) {
    CAW_LOG_ERROR("dmp_load_motion_driver_firmware failed");
    return -1;
  }
  if (dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation))) {
    CAW_LOG_ERROR("dmp_set_orientation failed");
    return 0;
  }

  if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                         DMP_FEATURE_ANDROID_ORIENT |
                         DMP_FEATURE_SEND_RAW_ACCEL |
                         DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL)) {
  }
  if (dmp_set_fifo_rate(DEFAULT_MPU_HZ)) {
    CAW_LOG_ERROR("dmp_set_fifo_rate failed");
    return -1;
  }

  long gyro[3], accel[3];
  if (mpu_run_self_test(gyro, accel) != 0x3) {
    CAW_LOG_ERROR("mpu_run_self_test failed");
    return -1;
  }
  if (mpu_set_dmp_state(1)) {
    CAW_LOG_ERROR("mpu_set_dmp_state failed");
    return -1;
  }
  return 0;
}

/**
 * @description: 获取DMP处理后的数据
 * @param {float*} pitch 俯仰角
 * @param {float*} roll 翻滚角
 * @param {float*} yaw 航向角
 * @return {*} 0：成功
 */
int MPU6050_GetData(float* pitch, float* roll, float* yaw) {
  float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
  unsigned long sensor_timestamp;
  short gyro[3], accel[3], sensors;
  unsigned char more;
  long quat[4];
  while (dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more) !=
         0)
    ;
  if (sensors & INV_WXYZ_QUAT) {
    q0 = quat[0] / q30;  // q30格式转换为浮点数
    q1 = quat[1] / q30;
    q2 = quat[2] / q30;
    q3 = quat[3] / q30;
    // 计算得到俯仰角/横滚角/航向角
    *pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;  // pitch
    *roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) *
            57.3;  // roll
    *yaw =
        atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) *
        57.3;  // yaw
  } else {
    CAW_LOG_WARN("INV_WXYZ_QUAT failed");
    return -1;
  }

  return 0;
}