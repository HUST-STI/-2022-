#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_



#define BLUE_USART_B USART2 //蓝牙传输使用的串口
#define BLUE_USART_S huart2 //蓝牙传输使用的串口

void Bluetooth_Init(void);
uint8_t Bluetooth_Self_Check(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif

