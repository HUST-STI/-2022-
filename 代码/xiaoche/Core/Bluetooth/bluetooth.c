#include "bluetooth.h"

uint8_t Rx2Buffer[5] = {0};//串口2接收数据存放的变量
uint8_t tar_point[4] = {0};//存放目标点的数组
extern int8_t tarspeed_left,tarspeed_right; 
uint8_t get_mode = 0; 

/*********************************************
 * 功能：蓝牙的初始化
 * 输入：无
 * 输出：无
 * ********************************************/
void Bluetooth_Init(void)
{
    HAL_UART_Receive_IT(&huart2, Rx2Buffer, 5);//开启串口2接收中断
}

/*********************************************
 * 功能：蓝牙传输自检
 * 输入：无
 * 输出：1-自检通过 0-自检不通过
 * ********************************************/
uint8_t Bluetooth_Self_Check(void)
{
    printf("bl check\r\n");
    while(1)
    {
        HAL_Delay(50);//不Delay用不了 不知道为啥
        if(Rx2Buffer[0] == 49 && Rx2Buffer[1] == 50)
        {
            printf("finish\r\n");
            break;
        }
    }
    
}


/********************************************************
 * 串口2接收中断函数
 * ******************************************************/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void BLUETOOTH_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==BLUE_USART_B)//如果是串口2
	{
        //printf("in ru\r\n");
        if(Rx2Buffer[0] == 2+48 || Rx2Buffer[0] == 3+48)//一个点
        {
            get_mode = Rx2Buffer[0]-48;
            tar_point[0] = Rx2Buffer[1]-48+1;
            tar_point[1] = Rx2Buffer[2]-48;
        }
        else if(Rx2Buffer[0] == 4+48)//两个点
        {
            get_mode = Rx2Buffer[0]-48;
            tar_point[0] = Rx2Buffer[1]-48+1;
            tar_point[1] = Rx2Buffer[2]-48;
            tar_point[2] = Rx2Buffer[3]-48+1;
            tar_point[3] = Rx2Buffer[4]-48;
        }
        else if(Rx2Buffer[0] == 5+48 || Rx2Buffer[0] == 6+48 || Rx2Buffer[0] == 7+48)//两个点
        {
            get_mode = Rx2Buffer[0]-48;
            tar_point[0] = Rx2Buffer[1]-48+1;
            tar_point[1] = Rx2Buffer[2]-48;
            tar_point[2] = Rx2Buffer[3]-48+1;
            tar_point[3] = Rx2Buffer[4]-48;
        }
        //         for(uint8_t k = 0;k<5;k++)
        // {
        //     printf("%d ",Rx2Buffer[k]);
        // }
        // printf("\r\n");
        OLED_ShowNum(0,48,Rx2Buffer[0]-48,1,16);
        OLED_ShowNum(16,48,Rx2Buffer[1]-48,1,16);
        OLED_ShowNum(32,48,Rx2Buffer[2]-48,1,16);
        OLED_ShowNum(48,48,Rx2Buffer[3]-48,1,16);
        OLED_ShowNum(64,48,Rx2Buffer[4]-48,1,16);
        OLED_Refresh();
        HAL_UART_Receive_IT(&huart2, Rx2Buffer, 5);//清除中断标志位
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
