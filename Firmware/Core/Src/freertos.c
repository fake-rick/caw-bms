/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bms.h"
#include "bq76920.h"
#include "code.h"
#include "device.h"
#include "event.h"
#include "pingpong.h"
#include "usart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
device_t uart_device;
event_t event;
bq76920_t afe;
/* USER CODE END Variables */
osThreadId bmsTaskHandle;
osThreadId timerTaskHandle;
osThreadId stateTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
int reset_device();
/* USER CODE END FunctionPrototypes */

void StartBMSTask(void const * argument);
void StartTimerTask(void const * argument);
void StartStateTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  device_init(&uart_device);
  device_set_id(&uart_device, 0x01, device_type_bms);
  event_init(&uart_device);
  event_register(main_code_system, sub_code_system_ping, pingpong_cb);
  event_run();
  bq76920_init(&afe, &hi2c1);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of bmsTask */
  osThreadDef(bmsTask, StartBMSTask, osPriorityNormal, 0, 128);
  bmsTaskHandle = osThreadCreate(osThread(bmsTask), NULL);

  /* definition and creation of timerTask */
  osThreadDef(timerTask, StartTimerTask, osPriorityNormal, 0, 128);
  timerTaskHandle = osThreadCreate(osThread(timerTask), NULL);

  /* definition and creation of stateTask */
  osThreadDef(stateTask, StartStateTask, osPriorityNormal, 0, 128);
  stateTaskHandle = osThreadCreate(osThread(stateTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartBMSTask */
/**
 * @brief  Function implementing the bmsTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartBMSTask */
void StartBMSTask(void const * argument)
{
  /* USER CODE BEGIN StartBMSTask */
  /* Infinite loop */
  for (;;) {
    bq76920_step(&afe);
    bq76920_update_balance_cell(&afe);

    bq76920_sys_ctrl2_t ctl2;
    bq76920_sys_ctrl2(&afe, &ctl2);

    bq76920_cellbal1_t cellbal;
    bq76920_cellbal1(&afe, &cellbal);

    protocol_bms_t bms;
    bq76920_sys_stat_byte(&afe, &(bms.state));
    for (int i = 0; i < 5; i++) {
      bms.cell_voltage[i] = afe.cell_voltage[i] * 100;
      bms.balance[i] = *(((uint8_t *)&cellbal) + i);
    }
    bms.voltage = afe.v_pack * 100;
    bms.current = afe.current / 1000.0 * 100;
    bms.temperature = afe.temperature * 100;
    bms.soc = 0;
    bms.soh = 0;
    bms.dsg = ctl2.DSG_ON;
    bms.chg = ctl2.CHG_ON;
    protocol_write_bms_info(&uart_device, &bms);
    osDelay(100);
  }
  /* USER CODE END StartBMSTask */
}

/* USER CODE BEGIN Header_StartTimerTask */
/**
 * @brief Function implementing the timerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTimerTask */
void StartTimerTask(void const * argument)
{
  /* USER CODE BEGIN StartTimerTask */
  /* Infinite loop */
  for (;;) {
    event_timer();
    osDelay(1000);
  }
  /* USER CODE END StartTimerTask */
}

/* USER CODE BEGIN Header_StartStateTask */
/**
 * @brief Function implementing the stateTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartStateTask */
void StartStateTask(void const * argument)
{
  /* USER CODE BEGIN StartStateTask */
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(LED_A_GPIO_Port, LED_A_Pin);
    osDelay(500);
  }
  /* USER CODE END StartStateTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

