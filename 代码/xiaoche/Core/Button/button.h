#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "stm32f1xx.h"

#define BUTTON_1_GPIO_PORT GPIOB
#define BUTTON_2_GPIO_PORT GPIOB
#define BUTTON_3_GPIO_PORT GPIOB

#define BUTTON_1_GPIO_PIN GPIO_PIN_13
#define BUTTON_2_GPIO_PIN GPIO_PIN_14
#define BUTTON_3_GPIO_PIN GPIO_PIN_15

uint8_t Button_1_Check(void);
uint8_t Button_2_Check(void);
uint8_t Button_3_Check(void);


#endif
