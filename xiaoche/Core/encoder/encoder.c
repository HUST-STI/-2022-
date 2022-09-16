#include "encoder.h"

Motor motor_right,motor_left;
uint8_t voidErr = 0;

/***********************************
 * 功能：进行电机定时器相关的初始化
 * 输入：无
 * 返回：无
 * ********************************/
void Motor_Init(void)
{
	HAL_TIM_Encoder_Start(&ENCODER_TIM_1, TIM_CHANNEL_ALL);      //开启编码器定时器
    __HAL_TIM_ENABLE_IT(&ENCODER_TIM_1,TIM_IT_UPDATE);           //开启编码器定时器更新中断,防溢出处理
    HAL_TIM_Encoder_Start(&ENCODER_TIM_2, TIM_CHANNEL_ALL);      //开启编码器定时器
    __HAL_TIM_ENABLE_IT(&ENCODER_TIM_2,TIM_IT_UPDATE);           //开启编码器定时器更新中断,防溢出处理

	HAL_TIM_Base_Start_IT(&GAP_TIM);                       //开启50ms定时器中断
	HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_1);            //开启PWM
    HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_2);            //开启PWM
    HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_3);            //开启PWM
    HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_4);            //开启PWM
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM_1, 10000);                //编码器定时器初始值设定为10000
    __HAL_TIM_SET_COUNTER(&ENCODER_TIM_2, 10000);                //编码器定时器初始值设定为10000

    motor_right.lastCount = 10000;                                   //结构体内容初始化
    motor_right.totalCount = 10000;
	motor_right.overflowNum = 0;                                   
    motor_right.speed = 0;
    motor_right.direct = 0;
    motor_left.lastCount = 10000;                                   //结构体内容初始化
    motor_left.totalCount = 10000;
	motor_left.overflowNum = 0;                                   
    motor_left.speed = 0;
    motor_left.direct = 0;
}

/***************************************
 * 功能：测量一次电机的速度
 * 输入：无
 * 返回：无
 * ************************************/
void Motor_Get_Speed(TIM_HandleTypeDef *htim)
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器回调函数，用于计算速度
{	
	if(htim->Instance==ENCODER_TIM_1.Instance)//编码器输入定时器溢出中断		              
	{		
		if(COUNTERNUM_1 < 10000) motor_right.overflowNum++;       //如果是向上溢出
        else if(COUNTERNUM_1 >= 10000) motor_right.overflowNum--; //如果是向下溢出
        //__HAL_TIM_SetCounter(&ENCODER_TIM_1, 10000);             //重新设定初始值
    }
    if(htim->Instance==ENCODER_TIM_2.Instance)//编码器输入定时器溢出中断		              
	{		
		if(COUNTERNUM_2 < 10000) motor_left.overflowNum++;       //如果是向上溢出
        else if(COUNTERNUM_2 >= 10000) motor_left.overflowNum--; //如果是向下溢出
        //__HAL_TIM_SetCounter(&ENCODER_TIM_2, 10000);             //重新设定初始值
    }
    if(htim->Instance==GAP_TIM.Instance)//间隔定时器中断，是时候计算速度了
    {
        motor_right.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM_1);//如果向上计数（正转），返回值为0，否则返回值为1
        motor_right.totalCount = COUNTERNUM_1 + motor_right.overflowNum * RELOADVALUE_1;//一个周期内的总计数值等于目前计数值加上溢出的计数值
        motor_right.speed = -((float)(motor_right.totalCount - motor_right.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * 600);//算得每秒多少转,除以4是因为4倍频
        motor_right.lastCount = motor_right.totalCount; //记录这一次的计数值

        motor_left.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM_2);//如果向上计数（正转），返回值为0，否则返回值为1
        motor_left.totalCount = COUNTERNUM_2 + motor_left.overflowNum * RELOADVALUE_2;//一个周期内的总计数值等于目前计数值加上溢出的计数值
        motor_left.speed = (float)(motor_left.totalCount - motor_left.lastCount) / (4 * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * 600;//算得每秒多少转,除以4是因为4倍频
        motor_left.lastCount = motor_left.totalCount; //记录这一次的计数值
	}
    //printf("motor_right.speed = %f\r\n",motor_right.speed);
    //printf("motor_left.speed = %f\r\n",motor_left.speed);
}
