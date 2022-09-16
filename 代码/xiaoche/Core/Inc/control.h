#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "stm32f1xx.h"
#include "encoder.h"
#include "PID.h"
#include "tim.h"
#include "usart.h"
#include "main.h"
#include "g_sensor.h"
#include "oled.h"
#include "JY901.h"
#include "bluetooth.h"


#define LIFE_LED_GPIO_PORT GPIOC
#define LIFE_LED_GPIO_PIN GPIO_PIN_13
#define LIFE_LED_TIME_GAP 40

#define FORWARD_GAP_Y 16
#define FORWARD_GAP_X 17

#define FOR_SPEED 70

void Life_Led(void);
void run_one(uint8_t x_scope,uint8_t y_scope);
void run_three(uint8_t x_scope,uint8_t y_scope);
void run_four(uint8_t x_scope,uint8_t y_scope,uint8_t x_scope_2,uint8_t y_scope_2);
void run_five(void);
void run_six(void);
void run_seven(void);

#endif
