#ifndef BC_FREERTOS_PID_H
#define BC_FREERTOS_PID_H
#endif //BC_FREERTOS_PID_H

typedef struct {
    double Gx;              //陀螺仪x轴角速度
    double Gy;              //陀螺仪y轴角速度
    double Gz;              //陀螺仪z轴角速度
    double KalmanAngleX;    //x轴角度
    double KalmanAngleY;    //y轴角度
}MPU6050_data;

typedef struct{
    int encoder_left;
    int encoder_right;
    float angle_x;
    float gypo_x;
}queue_msg;

int PD_Control(float angle_x, float Gypo_x);
int PI_Control(int encoder_left,int encoder_right);
int tur_Control(int encoder_left, int encoder_right, float Gypo_z);
int car_balance_oledshow(int encoder_left,int encoder_right, float angle_x, float Gypo_x );