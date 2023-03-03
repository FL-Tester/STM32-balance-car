/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stdio.h"
#include "stdlib.h"
#include "encoder.h"
#include "usart.h"
#include "tb6612.h"
#include "adc.h"
#include "ssd1306.h"
#include "nodmp_mpu6050.h"
#include "string.h"
#include "car_bl_pid.h"
#define debug 0
#if 0  //DMP已经导入 但是flash好像空间不够
#include "../../Drivers/MPU6050_DMP//mpu6050.h"
    MPU6050_initialize();
    DMP_Init();
    Read_DMP();
    printf("%f\r\n",Pitch);
#endif

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
//读取数据任务
osThreadId_t readDataTaskHandle;
const osThreadAttr_t readDataTask_attributes = {
        .name = "readDataTask",
        .stack_size = 128 * 8,
        .priority = (osPriority_t) osPriorityNormal + 1,
};

//数据处理任务
osThreadId_t dataProcessTaskHandle;
const osThreadAttr_t dataProcessTask_attributes = {
        .name = "dataProcessTask",
        .stack_size = 128 * 8,
        .priority = (osPriority_t) osPriorityNormal + 2,
};

//队列
osMessageQueueId_t dataQueueHandle;
const osMessageQueueAttr_t dataQueue_attributes = {
        .name = "dataQueue"
};

//互斥锁
osMutexId_t dataMutexHandle;
const osMutexAttr_t dataMutex_attributes = {
        .name = "UartMutex"
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal + 3,
};


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void ReadDataTask(void *argument);
void DataProcessTask(void *argument);
void ReadData(queue_msg *data,  MPU6050_t *MPU6050);
void OledShowInit(void);
/* USER CODE END FunctionPrototypes */
void StartDefaultTask(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
    dataMutexHandle = osMutexNew(&dataMutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  dataQueueHandle = osMessageQueueNew(1, sizeof(queue_msg), &dataQueue_attributes);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  readDataTaskHandle = osThreadNew(ReadDataTask, NULL, &readDataTask_attributes);
  dataProcessTaskHandle = osThreadNew(DataProcessTask, NULL, &dataProcessTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  * @dicrption:  初始化任务
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    MPU6050_Init(&hi2c2);     // 初始化mpu6050
    TB6612Init();                   // 初始化电机驱动(pwm)
    EncoderInit();                  // 初始化编码器(encoder)
    ssd1306_init();                 // 初始化OLED
    OledShowInit();                 // 初始化OLED显示
    osThreadExit();
    for (;;){
        ;
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
//读取处理显示任务(mpu6050数据 电机编码器数据)
void ReadDataTask(void *argument)
{
  /* USER CODE BEGIN ReadDataTask */
    /* Infinite loop */
    static queue_msg rdt_data = {
            .angle_x = 0,
            .gypo_x = 0,
            .encoder_right = 0,
            .encoder_left = 0
    };
    MPU6050_t MPU6050;
    for(;;){
        ReadData(&rdt_data, &MPU6050);
#if debug
        printf("angle_x :%f, gypo_x:%f\n", rdt_data.angle_x, rdt_data.gypo_x);
#endif
        osMessageQueuePut(dataQueueHandle, &rdt_data, 0, 0);
        osDelay(10);
    }
  /* USER CODE END ReadDataTask */
}
//数据处理任务 电机控制任务  pid控制 oled显示
void DataProcessTask(void *argument)
{
  /* USER CODE BEGIN DataProcessTask */
    static queue_msg dpt_data = {
            .angle_x = 0,
            .gypo_x = 0,
            .encoder_right = 0,
            .encoder_left = 0
    };
    for(;;){
        osMessageQueueGet(dataQueueHandle, &dpt_data, 0,osWaitForever);
        car_balance_oledshow(dpt_data.encoder_left, dpt_data.encoder_right, dpt_data.angle_x, dpt_data.gypo_x);
    }
  /* USER CODE END DataProcessTask */
}

void OledShowInit(void){
    ssd1306_set_cursor(32, 0);  //设置位置
    ssd1306_write_string(font7x10, "BlanceCar");
    ssd1306_set_cursor(0, 12);
    ssd1306_write_string(font7x10, "a_x:");
    ssd1306_set_cursor(64, 12);
    ssd1306_write_string(font7x10, "a_y:");
    ssd1306_set_cursor(5, 54);
    ssd1306_write_string(font7x10, "Design By FengLE!");
    ssd1306_update_screen();
}

void ReadData(queue_msg *data,  MPU6050_t *MPU6050){
    MPU6050_Read_All(&hi2c2, MPU6050);
    data->angle_x = MPU6050->KalmanAngleX;
    data->gypo_x = MPU6050->Gx;
    data->encoder_left = GetTim2Encoder();
    data->encoder_right = GetTim3Encoder();
}
/* USER CODE END Application */