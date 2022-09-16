#include "JY901.h"

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

char ACCCALSW[5] = {0XFF,0XAA,0X01,0X01,0X00};//进入加速度校准模式
char SAVACALSW[5]= {0XFF,0XAA,0X00,0X00,0X00};//保存当前配置

//static unsigned char TxBuffer[256];
uint8_t Rx3Buffer[11] = {0};//串口3接收缓冲
float angle_z = 0,angle_z_last = 0,angle_z_all = 0;
int32_t angle_z_temp = 0;
int32_t JY901_Init_val[4] = {0};
uint8_t init_count = 0;
int8_t jy901_of_count = 0;


//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
// void CopeSerial3Data(unsigned char ucData)
// {
// 	static unsigned char ucRxBuffer[250];
// 	static unsigned char ucRxCnt = 0;	
// 	//LED_REVERSE();					//接收到数据，LED灯闪烁一下
// 	ucRxBuffer[ucRxCnt++]=ucData;	//将收到的数据存入缓冲区中
// 	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
// 	{
// 		ucRxCnt=0;
// 		return;
// 	}
// 	if (ucRxCnt<11) {return;}//数据不满11个，则返回
// 	else
// 	{
// 		switch(ucRxBuffer[1])//判断数据是哪种数据，然后将其拷贝到对应的结构体中，有些数据包需要通过上位机打开对应的输出后，才能接收到这个数据包的数据
// 		{
// 			case 0x50:	memcpy(&stcTime,&ucRxBuffer[2],8);break;//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据结构体里面，从而实现数据的解析。
// 			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;
// 			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
// 			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
// 			case 0x54:	memcpy(&stcMag,&ucRxBuffer[2],8);break;
// 			case 0x55:	memcpy(&stcDStatus,&ucRxBuffer[2],8);break;
// 			case 0x56:	memcpy(&stcPress,&ucRxBuffer[2],8);break;
// 			case 0x57:	memcpy(&stcLonLat,&ucRxBuffer[2],8);break;
// 			case 0x58:	memcpy(&stcGPSV,&ucRxBuffer[2],8);break;
// 			case 0x59:	memcpy(&stcQ,&ucRxBuffer[2],8);break;
// 		}
// 		ucRxCnt=0;//清空缓存区
// 	}
// }

/*********************************
 * 功能：JY901的初始化函数
 * 输入：无
 * 返回：无
 * ******************************/
void JY901_Init(void)
{
    HAL_UART_Receive_IT(&JY901_USART_S, (uint8_t *)Rx3Buffer, 1);//清除中断标志位
}

/*********************************
 * 功能：JY901串口接收中断函数
 * 输入：无
 * 返回：无
 * ******************************/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void JY901_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==JY901_USART_B)//如果是串口3,JY901
    {
        angle_z = (float)((Rx3Buffer[7]<<8)|Rx3Buffer[6])/32768.0*180.0;
        angle_z_temp = angle_z;
        if(init_count<3 && angle_z_temp != 0)
        {
            JY901_Init_val[init_count++] = angle_z_temp;
        }
        else if(init_count == 3)
        {
            JY901_Init_val[init_count++] = (JY901_Init_val[0]+JY901_Init_val[1]+JY901_Init_val[2])/3;
        }
        HAL_UART_Receive_IT(&JY901_USART_S, (uint8_t *)Rx3Buffer, 11);//清除中断标志位
    }
}
