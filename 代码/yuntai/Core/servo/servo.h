#ifndef _SERVO_H_
#define _SERVO_H_

#include "head.h"

//舵机控制引脚宏定义
#define SERVO_LEFT_GPIO_PORT GPIOA
#define SERVO_RIGHT_GPIO_PORT GPIOA
#define SERVO_UP_GPIO_PORT GPIOA
#define SERVO_DOWN_GPIO_PORT GPIOB

#define SERVO_LEFT_GPIO_PIN GPIO_PIN_5
#define SERVO_RIGHT_GPIO_PIN GPIO_PIN_6
#define SERVO_UP_GPIO_PIN GPIO_PIN_7
#define SERVO_DOWN_GPIO_PIN GPIO_PIN_0

//定时器宏定义
#define SERVO_PWM_TIM htim2 //舵机PWM定时器
#define GAP_TIM htim1 //运算间隔
#define UP_TIM_CHANNEL TIM_CHANNEL_1//上面舵机对应的通道
#define DOWN_TIM_CHANNEL TIM_CHANNEL_2//上面舵机对应的通道

//舵机中值定义
#define SERVO_UP_CENTER 2500
#define SERVO_DOWN_CENTER 2500

void Servo_Init(void);
void Servo_Left_Check(void);
void Servo_Right_Check(void);
void Servo_Up_Check(void);
void Servo_Down_Check(void);

#endif