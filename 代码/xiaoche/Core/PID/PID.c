#include "pid.h"
PID pid_l_speed,pid_l_position,pid_r_speed,pid_r_position;
extern Motor motor_right,motor_left;
extern float angle_z,angle_z_last,angle_z_all;
extern int16_t JY901_Init_val[4];

/**********************************
 * 功能：PID结构体参数初始化
 * 输入：无
 * 返回：无
 * *******************************/
void PID_Init(void)//PID参数初始化
{
    pid_l_speed.err = 0;
    pid_l_speed.integral = 0;
    pid_l_speed.maxIntegral = 1000;
    pid_l_speed.maxOutput = __HAL_TIM_GetAutoreload(&PWM_TIM);
    pid_l_speed.lastErr = 0;
    pid_l_speed.output = 0;
    pid_l_speed.kp = KP_l_speed;
    pid_l_speed.ki = KI_l_speed;
    pid_l_speed.kd = KD_l_speed;

    pid_r_speed.err = 0;
    pid_r_speed.integral = 0;
    pid_r_speed.maxIntegral = 1000;
    pid_r_speed.maxOutput = __HAL_TIM_GetAutoreload(&PWM_TIM);
    pid_r_speed.lastErr = 0;
    pid_r_speed.output = 0;
    pid_r_speed.kp = KP_r_speed;
    pid_r_speed.ki = KI_r_speed;
    pid_r_speed.kd = KD_r_speed;

    pid_l_position.err = 0;
    pid_l_position.integral = 0;
    pid_l_position.maxIntegral = 80;
    pid_l_position.maxOutput = __HAL_TIM_GetAutoreload(&PWM_TIM);
    pid_l_position.lastErr = 0;
    pid_l_position.output = 0;
    pid_l_position.kp = KP_l_position;
    pid_l_position.ki = KI_l_position;
    pid_l_position.kd = KD_l_position;

    pid_r_position.err = 0;
    pid_r_position.integral = 0;
    pid_r_position.maxIntegral = 80;
    pid_r_position.maxOutput = __HAL_TIM_GetAutoreload(&PWM_TIM);
    pid_r_position.lastErr = 0;
    pid_r_position.output = 0;
    pid_r_position.kp = KP_r_position;
    pid_r_position.ki = KI_r_position;
    pid_r_position.kd = KD_r_position;
}

/****************************************
 * 作用：速度环PID计算
 * 参数：PID参数结构体地址；目标值；反馈值
 * 返回值：无
 * ****************************************/
float Speed_PID_Realize(PID* pid,float target,float feedback)//一次PID计算
{
    // if(pid->err < 1 && pid->err > -1) pid->err = 0;//pid死区
    pid->err = target - feedback;
    pid->integral += pid->err;

    if(pid->ki * pid->integral < -pid->maxIntegral) pid->integral = -pid->maxIntegral / pid->ki;//积分限幅
    else if(pid->ki * pid->integral > pid->maxIntegral) pid->integral = pid->maxIntegral / pid->ki;

    //if(abs(pid->err - pid->lastErr) > 100) pid->integral = 0;

    pid->output = (pid->kp * pid->err) + (pid->ki * pid->integral) + (pid->kd * (pid->err - pid->lastErr));//全量式PID

    //输出限幅
    if(target >= 0)//正转时
    {
        if(pid->output < 0) pid->output = 0;
        else if(pid->output > pid->maxOutput) pid->output = pid->maxOutput;
    }
    else if(target < 0)//反转时
    {
        if(pid->output < -pid->maxOutput) pid->output = -pid->maxOutput;
        else if(pid->output > 0) pid->output = 0;
    }

    pid->lastErr = pid->err;

    return pid->output;
}

/****************************************
 * 作用：位置环PID计算
 * 参数：PID参数结构体地址；目标值；反馈值
 * 返回值：无
 * ****************************************/
float Location_PID_Realize(PID* pid,float target,float feedback)//一次PID计算
{
     if(pid->err < 3 && pid->err > -3) pid->err = 0;//pid死区
    pid->err = target - feedback;
    pid->integral += pid->err;

    if(pid->ki * pid->integral < -pid->maxIntegral) pid->integral = -pid->maxIntegral / pid->ki;//积分限幅
    else if(pid->ki * pid->integral > pid->maxIntegral) pid->integral = pid->maxIntegral / pid->ki;

    pid->output = (pid->kp * pid->err) + (pid->ki * pid->integral) + (pid->kd * (pid->err - pid->lastErr));//全量式PID

    //输出限幅
    if(pid->output > pid->maxOutput) pid->output = pid->maxOutput;
    if(pid->output < -pid->maxOutput) pid->output = -pid->maxOutput;

    pid->lastErr = pid->err;

    return pid->output;
}

/****************************************
 * 作用：设置左边电机速度
 * 参数：PID参数结构体地址；目标值；反馈值
 * 返回值：无
 * ****************************************/
void Set_Left_Speed(float target,int16_t ad_speed)
{
    float pid_out = 0;
    if(target >= 180) target = 180;
    if(target < -180) target = -180;
    target = target + ad_speed;
    pid_out = Speed_PID_Realize(&pid_l_speed,target,motor_left.speed);
    //printf("pid_out = %.1f\r\n",pid_out);
    //printf("ad_speed = %d\r\n",ad_speed);
    //printf("pid_i = %.2f\r\n",pid_l_speed.integral*pid_l_speed.ki);
    //pid_out = ad_speed+pid_out;
    //printf("pid_out = %.1f\r\n",pid_out);
    if(pid_out >= 0) 
    {
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_1, pid_out);//修改电机PWM占空比
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_2, 0);//修改电机PWM占空比
    }
    else if(pid_out < 0) 
    {
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_1, 0);//修改电机PWM占空比
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_2, pid_out);//修改电机PWM占空比
    }
}

/****************************************
 * 作用：设置右边电机速度
 * 参数：PID参数结构体地址；目标值；反馈值
 * 返回值：无
 * ****************************************/
void Set_Right_Speed(float target,int16_t ad_speed)
{
    float pid_out = 0;
    if(target >= 180) target = 180;
    if(target < -180) target = -180;
	target = target + ad_speed;
    pid_out = Speed_PID_Realize(&pid_r_speed,target,motor_right.speed);
    //pid_out = ad_speed+pid_out;
    if(pid_out >= 0) 
    {
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_3, pid_out);//修改电机PWM占空比
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_4, 0);//修改电机PWM占空比
    }
    else if(pid_out < 0) 
    {
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_3, 0);//修改电机PWM占空比
        __HAL_TIM_SetCompare(&PWM_TIM, TIM_CHANNEL_4, pid_out);//修改电机PWM占空比
    }

}

/****************************************
 * 作用：位置PID设置小车行驶距离
 * 参数：行驶距离，差速
 * 返回值：无
 * ****************************************/
void Set_Left_Location(float distance)
{
    float tar_speed = 0;
    tar_speed = Location_PID_Realize(&pid_l_position,distance,motor_left.totalCount);
    //if(tar_speed > 85) tar_speed = 85;
    //if(tar_speed < -85) tar_speed = -85;
    Set_Left_Speed(tar_speed,0);
}

/****************************************
 * 作用：位置PID设置小车行驶距离
 * 参数：行驶距离，差速
 * 返回值：无
 * ****************************************/
void Set_Right_Location(float distance)
{
    float tar_speed = 0; 
    tar_speed = Location_PID_Realize(&pid_r_position,distance,motor_right.totalCount);    
    //if(tar_speed > 85) tar_speed = 85;		
    //if(tar_speed < -85) tar_speed = -85;
    Set_Right_Speed(tar_speed,0);
}

/****************************************
 * 作用：位置PID设置小车转弯
 * 参数：行驶距离，差速
 * 返回值：无
 * ****************************************/
void Set_Turn_Angle(float angle)
{
    float tar_speed = 0;
    tar_speed = Location_PID_Realize(&pid_l_position,angle,angle_z_all);
    //printf("ta%.1f\r\n",angle);
    if(tar_speed > FOR_SPEED) tar_speed = FOR_SPEED;
    if(tar_speed < -FOR_SPEED) tar_speed = -FOR_SPEED;
    //printf("aa%.1f\r\n",angle_z_all);
    Set_Left_Speed(-tar_speed,0);
    Set_Right_Speed(tar_speed,0);

    
}

