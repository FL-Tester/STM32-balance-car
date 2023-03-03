//
// Created by FengLE on 2023/1/28.
//

#include "tb6612.h"
#include "tim.h"
#include "gpio.h"
#include "stdlib.h"

//初始化
void TB6612Init(void){
    MX_GPIO_Init();
    MX_TIM4_Init();
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

//左电机
void Motor_Left(DIRECTION Direction){
    switch(Direction){
        case FORWARD:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        case BACKWARD:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
            break;
        case STOP:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}
//右电机
void Motor_Right(DIRECTION Direction){
    switch(Direction){
        case FORWARD:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            break;
        case BACKWARD:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
            break;
        case STOP:
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
            HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

void SetMotorDir(MOTOR Motor, DIRECTION Direction){
    switch(Motor){
        case LEFT:
            Motor_Left(Direction);
            break;
        case RIGHT:
            Motor_Right(Direction);
            break;
        default:
            break;
    }
}

//限幅
void MotorLimit(int *iSpeed){
    if(*iSpeed > MAX_SPEED){
        *iSpeed = MAX_SPEED;
    }
    if(*iSpeed < MIN_SPEED){
        *iSpeed = MIN_SPEED;
    }
}

//设置电机速度
void SetMotorSpeed(int iSpeed){
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, abs(iSpeed));
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, abs(iSpeed));
}
