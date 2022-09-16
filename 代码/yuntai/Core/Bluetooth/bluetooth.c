#include "bluetooth.h"

uint8_t Rx2Buffer = 0;//串口2接收数据存放的变量

/*********************************************
 * 功能：蓝牙的初始化
 * 输入：无
 * 输出：无
 * ********************************************/
void Bluetooth_Init(void)
{
    HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx2Buffer, 1);//开启串口2接收中断
}

/*********************************************
 * 功能：蓝牙传输自检
 * 输入：无
 * 输出：1-自检通过 0-自检不通过
 * ********************************************/
uint8_t Bluetooth_Self_Check(void)
{
    uint8_t bt_send = 1;
    HAL_UART_Transmit(&BLUE_USART_S, &bt_send, 1, 50);
    HAL_Delay(200);
    if(Rx2Buffer == 11)//如果接收到反馈信号
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/********************************************************
 * 串口2接收中断函数
 * ******************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==BLUE_USART_B)//如果是串口1
	{

        HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx2Buffer, 1);//清除中断标志位
	}	
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

int fgetc(FILE *f)
{	
	int ch;
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}
