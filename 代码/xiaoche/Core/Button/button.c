#include "button.h"

/********************************
 * 功能：检查按键1是否按下
 * 输入：无
 * 返回：1-按键1按下 0-按键1未按下
 * ******************************/
uint8_t Button_1_Check(void)
{
    if(HAL_GPIO_ReadPin(BUTTON_1_GPIO_PORT,BUTTON_1_GPIO_PIN)==SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/********************************
 * 功能：检查按键2是否按下
 * 输入：无
 * 返回：1-按键1按下 0-按键1未按下
 * ******************************/
uint8_t Button_2_Check(void)
{
    if(HAL_GPIO_ReadPin(BUTTON_2_GPIO_PORT,BUTTON_2_GPIO_PIN)==SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/********************************
 * 功能：检查按键3是否按下
 * 输入：无
 * 返回：1-按键1按下 0-按键1未按下
 * ******************************/
uint8_t Button_3_Check(void)
{
    if(HAL_GPIO_ReadPin(BUTTON_3_GPIO_PORT,BUTTON_3_GPIO_PIN)==SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

