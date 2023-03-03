#ifndef BC_FREERTOS_TB6612_H
#define BC_FREERTOS_TB6612_H

#endif //BC_FREERTOS_TB6612_H

#define MAX_SPEED 3600
#define MIN_SPEED -3600

//方向
typedef enum{
    FORWARD = 0,
    BACKWARD = 1,
    STOP = 2
}DIRECTION;

//左右电机
typedef enum{
    LEFT = 0,
    RIGHT = 1
}MOTOR;

void TB6612Init(void);
void Motor_Left(DIRECTION Direction);
void Motor_Right(DIRECTION Direction);
void SetMotorDir(MOTOR Motor, DIRECTION Direction);
void SetMotorSpeed(int iSpeed);
void MotorLimit(int *iSpeed);

