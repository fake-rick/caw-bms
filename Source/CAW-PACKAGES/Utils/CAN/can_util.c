#include "can_util.h"

#include "can.h"
#include "caw_status.h"

/**
 * @description: CAN总线初始化
 * @return {*}
 */
int CAN_Init(void) {
  // 过滤器配置
  CAN_FilterTypeDef filter;
  filter.FilterBank = 0;                       // 使用0号过滤器组
  filter.FilterMode = CAN_FILTERMODE_IDMASK;   // 使用掩码模式
  filter.FilterScale = CAN_FILTERSCALE_32BIT;  // 使用32位长度
  filter.FilterIdHigh = 0x0000;                // ID 高字节
  filter.FilterIdLow = 0x0000;                 // ID低字节
  filter.FilterMaskIdHigh = 0x0000;            // 掩码高字节
  filter.FilterMaskIdLow = 0x0000;             // 掩码低字节
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;  // 关联FIFO
  filter.FilterActivation = ENABLE;            // 使能过滤器
  if (HAL_CAN_ConfigFilter(&hcan1, &filter)) {
    return CAW_ERR;
  }
  // if (HAL_CAN_ConfigFilter(&hcan2, &filter)) {
  //   return CAW_ERR;
  // }

  // 使能CAN FIFO_0接收中断
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING)) {
    return CAW_ERR;
  }
  // if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING)) {
  //   return CAW_ERR;
  // }

  // 启动CAN
  if (HAL_CAN_Start(&hcan1)) {
    return CAW_ERR;
  }
  // if (HAL_CAN_Start(&hcan2)) {
  //   return CAW_ERR;
  // }
  return CAW_OK;
}