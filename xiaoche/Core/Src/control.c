#include "control.h"
int8_t life_led_count = 0,life_led_state = 1;
extern PID pid_l_speed,pid_l_position,pid_r_speed,pid_r_position;
extern Motor motor_right,motor_left;
int8_t tarspeed_left = 0,tarspeed_right = 0; 

extern uint8_t Rx2Buffer[5];//串口2接收数据存放的变量
extern float angle_z,angle_z_all;
extern uint32_t angle_z_temp;
extern int32_t JY901_Init_val[4];
extern uint8_t run_mode,run_mode_temp;
uint16_t time_count = 0,time_count_flag = 0;
extern int8_t jy901_of_count;
uint8_t speed_flag = 0,angle_flag = 0;
int32_t tar_angle = 0;
extern uint8_t init_count;
uint8_t step_count = 1;
extern int32_t tar_direct;
uint8_t angle_once = 1;
uint8_t time_change = 0;

/***************************************
 * 串口接收中断函数
 * USART1: 115200
 * USART2: 9600
 * USART3: 9600
 * ************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //JY901_UART_RxCpltCallback(huart);//串口3 JY901
    BLUETOOTH_UART_RxCpltCallback(huart);//串口2 蓝牙
}

/***************************************
 * 定时器中断函数
 * 频率：20Hz
 * ************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    Motor_Get_Speed(htim);//得到电机转速
    if(htim->Instance==GAP_TIM.Instance)
    {
        //Life_Led();//LED生命指示灯
        life_led_count++;
        //printf("%.2f",motor_left.speed);
        //printf(",");
        //printf("%.2f\r\n",motor_right.speed);
        //printf("motor_right.totalCount = %d\r\n",motor_right.totalCount);
        //printf("motor_right.overflowNum = %d\r\n",motor_right.overflowNum);
        
        if(speed_flag == 1)
        {
            Set_Right_Speed(tarspeed_left,-((angle_z_temp - tar_direct)*1));
            Set_Left_Speed(tarspeed_right,(angle_z_temp - tar_direct)*1);
        }
        else if(angle_flag == 1)
        {
            Set_Turn_Angle(tar_angle);
        }
        //printf("al%.1f\r\n",angle_z_all);
        //printf("tr%d,",tar_angle);
        //printf("td%d\r\n",tar_direct);
        //printf("in = %d\r\n",JY901_Init_val[3]);
        //printf("count = %d\r\n",init_count);
        //printf("%.1f\r\n",(float)(angle_z_temp - JY901_Init_val[3]));
        //printf("tar_angle = %d\r\n",tar_angle);
        //printf("%.1f\r\n",angle_z_all);
        //printf("ct%d,",step_count);
        //printf("di%d,",tar_direct);
        //printf("temp%d\r\n",angle_z_temp);
        // for(uint8_t k = 0;k<5;k++)
        // {
        //     printf("%d ",Rx2Buffer[k]);
        // }
        // printf("\r\n");
        //printf("mode%d\r\n",run_mode);
        // if(run_mode != 0)
        // {
        //     printf("st%d\r\n",step_count);
        // }
        
        if(time_count_flag == 1)
            time_count++;
    } 
    
}

/*********************************
 * 功能：板载LED闪烁，生命指示灯
 * 输入：无
 * 返回值：无
 * *******************************/
void Life_Led(void)
{
    if(life_led_count < LIFE_LED_TIME_GAP) life_led_count++;
    else if(life_led_count == LIFE_LED_TIME_GAP)
    {
        life_led_state = -life_led_state;
        life_led_count = 1;
    }
    if(life_led_state == 1)
    {
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_RESET);//输出低
    }
    else if(life_led_state == -1)
    {
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    }
}

/*********************************
 * 功能：基础部分1、2问运行函数
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_one(uint8_t x_scope,uint8_t y_scope)
{
    
    if(x_scope > 4)x_scope = 4;//防止超出场地
    if(y_scope > 3)y_scope = 3;
    //横着走x_scope格
    if((time_count <= FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        //tar_direct = JY901_Init_val[3];
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        if(y_scope != 0)step_count++;
        else if(y_scope == 0)
        {
            step_count = 0;
            run_mode = 0;
            tarspeed_left = 0;tarspeed_right = 0;
            time_count_flag = 0;
            time_count = 0;
        }
    } 

    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;

    }

    //竖着着走y_scope格
    if((time_count <= FORWARD_GAP_Y*y_scope) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*y_scope) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        //speed_flag = 0;angle_flag = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    }
    
}


/*********************************
 * 功能：基础部分第3问运行函数
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_three(uint8_t x_scope,uint8_t y_scope)
{
    if(x_scope > 4)x_scope = 4;//防止超出场地
    if(y_scope > 3)y_scope = 3;
    //////////////////////////////前进部分和前面一样////////////////////////////
    //横着走x_scope格
    if((time_count <= FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        //tar_direct = JY901_Init_val[3];
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        if(y_scope == 0)
        {
            HAL_Delay(2000);
            step_count = 7;
        }
        else
            step_count++;
    } 

    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }

    //竖着着走y_scope格
    if((time_count <= FORWARD_GAP_Y*y_scope) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*y_scope) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        HAL_Delay(2000);
        step_count++;
    }
    /////////////////////////////返回部分//////////////////////////

    //倒车竖着着走y_scope格
    if((time_count <= FORWARD_GAP_Y*y_scope+4) && step_count == 4)
    {
        
        time_count_flag = 1;
        tarspeed_left = (-FOR_SPEED);tarspeed_right = (-FOR_SPEED);
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*y_scope+4) && step_count == 4)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //右转90°
    if(step_count == 5 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 5 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走x_scope格
    if((time_count <= FORWARD_GAP_X*x_scope-2) && step_count == 6)
    {
        //tar_direct = JY901_Init_val[3];
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*x_scope-2) && step_count == 6)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
    } 

    //倒车横着走x_scope格
    if((time_count <= FORWARD_GAP_X*x_scope) && step_count == 7)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = -FOR_SPEED;tarspeed_right = -FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*x_scope) && step_count == 7)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count = 0;
        run_mode = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    } 

}

/*********************************
 * 功能：发挥部分运行函数
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_four(uint8_t x_scope,uint8_t y_scope,uint8_t x_scope_2,uint8_t y_scope_2)
{
    if(x_scope > 4)x_scope = 4;//防止超出场地
    if(y_scope > 3)y_scope = 3;
    if(x_scope_2 > 4)x_scope_2 = 4;
    if(y_scope_2 > 3)y_scope_2 = 3;
    //////////////////////////////第一段前进部分和前面一样////////////////////////////
    //横着走x_scope格
    if((time_count < FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        //tar_direct = JY901_Init_val[3];
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*x_scope) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 

    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) < 2)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
            pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }

    //竖着着走y_scope格
    if((time_count < FORWARD_GAP_Y*y_scope+3) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
	
    else if((time_count >FORWARD_GAP_Y*y_scope+3) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }


    //////////////////////////////第二段前进部分要考虑转向角////////////////////////////
    //右转或左转90°
    if(step_count == 4 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        if(x_scope_2 < x_scope)//左转
        {
            tar_direct = tar_direct - 90;
            tar_angle = angle_z_all - 90;   
        }
        else if(x_scope_2 > x_scope)//右转
        {
            tar_direct = tar_direct + 90;
            tar_angle = angle_z_all + 90;   
        }
        angle_once = 0;
    }
    else if(step_count == 4 && angle_once == 0 && abs(tar_angle - angle_z_all) < 2)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
            pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走x_scope_2格
    if((time_count < FORWARD_GAP_X*abs(x_scope_2-x_scope)) && step_count == 5)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*abs(x_scope_2-x_scope)) && step_count == 5)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //右转或左转90°
    if(step_count == 6 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        if(y_scope_2 > y_scope)//右转
        {
            tar_direct = tar_direct - 90;
            tar_angle = angle_z_all - 90;   
        }
        else if(y_scope_2 < y_scope)//左转
        {
            time_change = 4;
            tar_direct = tar_direct + 90;
            tar_angle = angle_z_all + 90;   
        }
        angle_once = 0;
    }
    else if(step_count == 6 && angle_once == 0 && abs(tar_angle - angle_z_all) < 2)
    {
        //HAL_Delay(200);
        pid_l_speed.integral = 0;//积分清零
            pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走y_scope格
    if((time_count < FORWARD_GAP_Y*abs(y_scope_2-y_scope)-time_change) && step_count == 7)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*abs(y_scope_2-y_scope)-time_change) && step_count == 7)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
        time_change = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    }

}

/*********************************
 * 功能：发挥部分运行函数（转三个弯的特殊情况一到C）
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_five(void)
{
    //横着走1格到0
    if((time_count <= FORWARD_GAP_X*1) && step_count == 1)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走1格到4
    if((time_count <= FORWARD_GAP_Y*1) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*1) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //左90°
    if(step_count == 4 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 4 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走1格到5
    if((time_count <= FORWARD_GAP_X*1) && step_count == 5)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1) && step_count == 5)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //右90°
    if(step_count == 6 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 6 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走2格到D
    if((time_count <= FORWARD_GAP_Y*2) && step_count == 7)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*2) && step_count == 7)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //右90°
    if(step_count == 8 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 8 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走1格到C
    if((time_count <= FORWARD_GAP_X*1) && step_count == 9)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1) && step_count == 9)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    } 
}

/*********************************
 * 功能：发挥部分运行函数（转三个弯的特殊情况二 到2）
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_six(void)
{
    //横着走1格到0
    if((time_count <= FORWARD_GAP_X*1) && step_count == 1)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走2格到8
    if((time_count <= FORWARD_GAP_Y*2) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*2) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //左90°
    if(step_count == 4 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 4 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走3格到B
    if((time_count <= FORWARD_GAP_X*3) && step_count == 5)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*3) && step_count == 5)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //左90°
    if(step_count == 6 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 6 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走2格到3
    if((time_count <= FORWARD_GAP_Y*2-3) && step_count == 7)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*2-3) && step_count == 7)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //左90°
    if(step_count == 8 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 8 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走1格到2
    if((time_count <= FORWARD_GAP_X*1-4) && step_count == 9)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1-4) && step_count == 9)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    } 
}

/*********************************
 * 功能：发挥部分运行函数（转三个弯的特殊情况三 到3）
 * 输入：无
 * 返回值：无
 * 
 * *******************************/
void run_seven(void)
{
    //横着走1格到0
    if((time_count <= FORWARD_GAP_X*1) && step_count == 1)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1) && step_count == 1)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //右转90°
    if(step_count == 2 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 2 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走1格到4
    if((time_count <= FORWARD_GAP_Y*1) && step_count == 3)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*1) && step_count == 3)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //左90°
    if(step_count == 4 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 4 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走2格到6
    if((time_count <= FORWARD_GAP_X*2) && step_count == 5)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*2) && step_count == 5)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    } 
    //左90°
    if(step_count == 6 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct + 90;
        tar_angle = angle_z_all + 90;
        angle_once = 0;
    }
    else if(step_count == 6 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //竖着着走1格到2
    if((time_count <= FORWARD_GAP_Y*1-2) && step_count == 7)
    {
        
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
        speed_flag = 1;angle_flag = 0;
    }
    else if((time_count >FORWARD_GAP_Y*1-2) && step_count == 7)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
    }
    //右90°
    if(step_count == 8 && angle_once == 1)
    {
        speed_flag = 0;angle_flag = 1;
        tar_direct = tar_direct - 90;
        tar_angle = angle_z_all - 90;
        angle_once = 0;
    }
    else if(step_count == 8 && angle_once == 0 && abs(tar_angle - angle_z_all) == 0)
    {
        pid_l_speed.integral = 0;//积分清零
        pid_r_speed.integral = 0;
        step_count++;
        angle_once = 1;
    }
    //横着走1格到3
    if((time_count <= FORWARD_GAP_X*1-4) && step_count == 9)
    {
        speed_flag = 1;angle_flag = 0;
        time_count_flag = 1;
        tarspeed_left = FOR_SPEED;tarspeed_right = FOR_SPEED;
    }
    else if((time_count >FORWARD_GAP_X*1-4) && step_count == 9)
    {
        time_count_flag = 0;
        time_count = 0;
        tarspeed_left = 0;tarspeed_right = 0;
        step_count++;
        step_count = 0;
        run_mode = 0;
        HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
    } 
}
