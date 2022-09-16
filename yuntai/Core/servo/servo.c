#include "servo.h"

int16_t servo_up_pwm = 0,servo_down_pwm = 0;

/************************************
 * 功能：舵机相关初始化，舵机归中
 * 输入：无
 * 输出：无
 * **********************************/
void Servo_Init(void)
{
    __HAL_TIM_SetCompare(&SERVO_PWM_TIM, UP_TIM_CHANNEL, SERVO_UP_CENTER);
    __HAL_TIM_SetCompare(&SERVO_DOWN_CENTER, DOWN_TIM_CHANNEL, SERVO_DOWN_CENTER);
}


/************************************
 * 功能：如果收到相应红外信号，舵机左转
 * 输入：无
 * 输出：无
 * **********************************/
void Servo_Left_Check(void)
{
    if(HAL_GPIO_ReadPin(SERVO_LEFT_GPIO_PORT,SERVO_LEFT_GPIO_PIN)==SET)
    {
        servo_down_pwm += 1;
    }
}

/************************************
 * 功能：如果收到相应红外信号，舵机右转
 * 输入：无
 * 输出：无
 * **********************************/
void Servo_Right_Check(void)
{
    if(HAL_GPIO_ReadPin(SERVO_RIGHT_GPIO_PORT,SERVO_RIGHT_GPIO_PIN)==SET)
    {
        servo_down_pwm -= 1;
    }
}

/************************************
 * 功能：如果收到相应红外信号，舵机上转
 * 输入：无
 * 输出：无
 * **********************************/
void Servo_Up_Check(void)
{
    if(HAL_GPIO_ReadPin(SERVO_UP_GPIO_PORT,SERVO_UP_GPIO_PIN)==SET)
    {
        servo_up_pwm -= 1;
    }
}

/************************************
 * 功能：如果收到相应红外信号，舵机上转
 * 输入：无
 * 输出：无
 * **********************************/
void Servo_Down_Check(void)
{
    if(HAL_GPIO_ReadPin(SERVO_DOWN_GPIO_PORT,SERVO_DOWN_GPIO_PIN)==SET)
    {
        servo_up_pwm += 1;
    }
}

//定时器中断函数，完成舵机控制
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==GAP_TIM.Instance)//20Hz运行一次
    {
        __HAL_TIM_SetCompare(&SERVO_PWM_TIM, UP_TIM_CHANNEL, SERVO_UP_CENTER + servo_up_pwm);
        __HAL_TIM_SetCompare(&SERVO_DOWN_CENTER, DOWN_TIM_CHANNEL, SERVO_DOWN_CENTER + servo_down_pwm);

    }
}
