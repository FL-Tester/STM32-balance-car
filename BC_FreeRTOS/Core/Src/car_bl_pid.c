//
// Created by FengLE on 2023/1/28.
//

#include "car_bl_pid.h"
#include "math.h"
#include "encoder.h"
#include "adc.h"
#include "tb6612.h"
#include "ssd1306.h"

static float mid_value = -1;       //小车平衡是角度 需要根据实际情况修改
static float Kp = 200;              //比例系数  确认极性为 +
static float Kd = 20;              //微分系数   极性为 +
static float Velocity_Kp = 0.2;       //速度环比例系数
static float Velocity_Ki = 0.001;     //速度环积分系数

//直立环PD控制函数声明 参数为角度和角速度
int PD_Control(float angle_x, float Gypo_x){
    float Bias;
    int balance;
    Bias = angle_x - mid_value;
    balance= Kp * Bias + Kd * Gypo_x;
    return balance;
}

//速度环PI控制函数声明 参数为两个编码器
int PI_Control(int encoder_left,int encoder_right){
    static float Velocity, Encoder_Least, Encoder, Movement;
    static float Encoder_Integral, Target_Velocity;
    Encoder_Least = (encoder_left + encoder_right) - 0;
    Encoder *= 0.7;
    Encoder += Encoder_Least*0.3;
    Encoder_Integral +=Encoder;  //积分时间 xms
    if(Encoder_Integral>10000)  	Encoder_Integral = 10000;  //限幅
    if(Encoder_Integral<-10000)	Encoder_Integral =- 10000;
    Velocity = Encoder*Velocity_Kp + Encoder_Integral*Velocity_Ki;
    return Velocity;
}

//转向控制函数声明  暂时没有适配蓝牙 控制小车运动 PD+PI 已经可以实现平衡了
int tur_Control(int encoder_left, int encoder_right, float Gypo_z){
    return 1;
}


//需要输入的参数是 x轴角度和角速度 以及编码器数据
int car_balance_oledshow(int encoder_left,int encoder_right, float angle_x, float Gypo_x ){
    //pid部分 控制电机
    int Balance = PD_Control(angle_x, Gypo_x);
    int Velocity = PI_Control(encoder_left, encoder_right);
    int Outpwm = Balance + Velocity;
    if (Outpwm > 0){
        SetMotorDir(LEFT, BACKWARD);
        SetMotorDir(RIGHT, BACKWARD);
    }else{

        SetMotorDir(LEFT, FORWARD);
        SetMotorDir(RIGHT, FORWARD);
    }
    MotorLimit(&Outpwm);  //限幅
    SetMotorSpeed(Outpwm);  //设置pwm

    //oled show  角度和adc采集的电压

}



