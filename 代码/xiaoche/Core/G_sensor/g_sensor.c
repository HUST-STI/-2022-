#include "g_sensor.h"

/**********************
 * 7.13基地材料柜左侧地图
 *    g1   g2   g3
 * 白 3500 3720 3610
 * 红 3340 3600 3410
 * **********************/ 

uint16_t g_sensor_value[3] = {0};//保存灰度传感器读取的模拟值
uint16_t g_sensor_value_new[3] = {0};//保存灰度传感器读取的模拟值
uint8_t alpha = 0.1;//一阶惯性滤波参数
uint16_t gs_white_standar[3] = {0};//保存标定白色得到的值
uint16_t gs_red_standar[3] = {0};//保存标定红色得到的值
uint8_t gs_1_check = 0,gs_2_check = 0,gs_3_check = 0;//保存传感器是否识别到红线

/***********************************
 * 功能：ADC灰度传感器传输初始化
 * 输入：无
 * 返回：无
 * **********************************/
void ADC_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);//ADC校准
}

/***********************************
 * 功能：进行一次ADC灰度传感器数值读取
 * 输入：无
 * 返回：无
 * **********************************/
void G_Sensor_Read(void)
{
    for(int i=0;i<3;i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,0xffff);
        g_sensor_value_new[i]=HAL_ADC_GetValue(&hadc1);
        g_sensor_value[i] = g_sensor_value_new[i] * (1-alpha) + g_sensor_value[i] * alpha;
    }

    Red_Line_Check();
    HAL_ADC_Stop(&hadc1);
}

/***********************************
 * 功能：进行巡线数值标定
 * 输入：无
 * 返回：无
 * **********************************/
void Line_Sensor_Check(void)
{
	/*
    uint16_t sensor1_value = 0,sensor2_value = 0,sensor3_value = 0;
    OLED_ShowString(0,0,(unsigned char *)"gs w check",16);//OLED显示状态
    printf("g_sensor white line check\r\n");
    do{}while(!Button_1_Check());//没有按下按键1就在此循环
    for(int i = 0;i<CHECH_TIMES;i++)//开始读取传感器均值进行标定    
    {
       G_Sensor_Read();//进行一次传感器读取
       sensor1_value += g_sensor_value[0];
       sensor2_value += g_sensor_value[1];
       sensor3_value += g_sensor_value[2];
    }
    gs_white_standar[0] = sensor1_value/CHECH_TIMES;
    gs_white_standar[1] = sensor2_value/CHECH_TIMES;
    gs_white_standar[2] = sensor3_value/CHECH_TIMES;
    printf("white line check finish\r\n");

    sensor1_value = 0;sensor2_value = 0;sensor3_value = 0;//缓冲变量清零

    printf("g_sensor red line check\r\n");
    do{}while(!Button_1_Check());//没有按下按键1就在此循环
    for(int i = 0;i<CHECH_TIMES;i++)//开始读取传感器均值进行标定    
    {
       G_Sensor_Read();//进行一次传感器读取
       sensor1_value += g_sensor_value[0];
       sensor2_value += g_sensor_value[1];
       sensor3_value += g_sensor_value[2];
    }
    gs_red_standar[0] = sensor1_value/CHECH_TIMES;
    gs_red_standar[1] = sensor2_value/CHECH_TIMES;
    gs_red_standar[2] = sensor3_value/CHECH_TIMES;
    printf("red line check finish\r\n");*/
}

/************************************
 * 功能：判断灰度传感器是否识别到红线
 * 输入：无
 * 返回：无
 * **********************************/
void Red_Line_Check(void)
{
    if( g_sensor_value[0] < (GS_1_WHITE_VALUE+GS_1_RED_VALUE) / 2)
    {
        gs_1_check = 1;
    }
    else
    {
        gs_1_check = 0;
    }

    if(g_sensor_value_new[1] < (GS_2_WHITE_VALUE+GS_2_RED_VALUE) / 2)
    {
        gs_2_check = 1;
    }
    else
    {
        gs_2_check = 0;
    }

    if(g_sensor_value_new[2] < (GS_3_WHITE_VALUE+GS_3_RED_VALUE) / 2)
    {
        gs_3_check = 1;
    }
    else
    {
        gs_3_check = 0;
    }
}

