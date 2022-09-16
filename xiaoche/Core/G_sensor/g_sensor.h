#ifndef _G_SENSOR_H_
#define _G_SENSOR_H_

#include "stm32f1xx.h"
#include "adc.h"
#include "oled.h"
#include <stdio.h>


#define CHECH_TIMES 5 //标定读取次数

//记录传感器对不同颜色的探测数值
#define GS_1_WHITE_VALUE 2400
#define GS_2_WHITE_VALUE 2800
#define GS_3_WHITE_VALUE 2400

#define GS_1_RED_VALUE 1900
#define GS_2_RED_VALUE 2000
#define GS_3_RED_VALUE 1500

void ADC_Init(void);
void G_Sensor_Read(void);
void Line_Sensor_Check(void);
void Red_Line_Check(void);

#endif

