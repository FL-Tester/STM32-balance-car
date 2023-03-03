//
// Created by FengLE on 2023/1/28.
//

#include "encoder.h"
#include "tim.h"

void EncoderInit(void){
    MX_TIM2_Init();
    MX_TIM3_Init();
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

int GetTim2Encoder(void){
    int iTim2Encoder =(short) __HAL_TIM_GET_COUNTER(&htim2); //强制换为short类型  就不用考虑正负了
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    return  iTim2Encoder;
}

int GetTim3Encoder(void){
    int iTim2Encoder = (short)__HAL_TIM_GET_COUNTER(&htim3);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    return  iTim2Encoder;
}