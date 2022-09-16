#ifndef _PID_H_
#define _PID_H_

#include "stm32f1xx.h"
#include "encoder.h"
#include <stdio.h>
#include "control.h"

//PID三个参数的值
#define KP_l_speed 9
#define KI_l_speed 2.5
#define KD_l_speed 0

#define KP_r_speed 9
#define KI_r_speed 2.5
#define KD_r_speed 0

#define KP_l_position 0.95
#define KI_l_position 0.0
#define KD_l_position 0

#define KP_r_position 0.95
#define KI_r_position 0.0
#define KD_r_position 0

#define pulse_per_circle 1040



#define LINE_CHANGE_C 8


typedef struct _PID//PID参数结构体
{
    float kp,ki,kd;
    float err,lastErr;
    float integral,maxIntegral; //积分值
    float output,maxOutput;
}PID;

void PID_Init(void);
float Speed_PID_Realize(PID* pid,float target,float feedback);//一次PID计算
float Location_PID_Realize(PID* pid,float target,float feedback);//一次PID计算
void Set_Left_Speed(float target,int16_t ad_speed);
void Set_Right_Speed(float target,int16_t ad_speed);
void Set_Left_Location(float distance);
void Set_Right_Location(float distance);
void Set_Turn(uint16_t angle_l,uint16_t angle_r);
void Set_Turn_Angle(float angle);


#endif
