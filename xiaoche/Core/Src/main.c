/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "bluetooth.h"
#include "oled.h"
#include "g_sensor.h"
#include "encoder.h" 
#include "PID.h"
#include "control.h"
#include "JY901.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void Car_All_Init(void);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern float angle_z,angle_z_last,angle_z_all;
extern int32_t angle_z_temp;
extern uint8_t init_count;
extern int32_t JY901_Init_val[4];
extern int8_t jy901_of_count;
uint8_t run_mode = 0,run_mode_temp = 1;
extern int32_t tar_angle;
extern  uint8_t speed_flag,angle_flag;
int32_t tar_direct = 0;
extern uint8_t get_mode;
extern uint8_t tar_point[4];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*********************************
* ��������DMA_Usart_Send
* ����˵�������ڷ��͹��ܺ���
*	�βΣ�buf, len
*	����ֵ����
*********************************
*/
void DMA_Usart_Send(uint8_t*buf, uint8_t len)
{
	if(HAL_UART_Transmit_DMA(&huart3, buf, len) != HAL_OK)  //�ж��Ƿ�����������������쳣������жϺ���
	{
		//printf("error tansmit");

		//Error_Handler();
	}
}
/*
*********************************
* ��������DMA_Usart_Read
* ����˵�������ڽ��չ��ܺ���
*	�βΣ�data, len
*	����ֵ����
*********************************
*/
void DMA_Usart_Read(uint8_t* data, uint8_t len)
{
	HAL_UART_Receive_DMA(&huart3, data, len);//���´�DMA����
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  Car_All_Init();
  printf("start\r\n");
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);  
	HAL_UART_Receive_DMA(&huart3, rx_buffer, BUFFER_SIZE);

  OLED_ShowString(0,0,(unsigned char *)"blue checking",16);
  OLED_Refresh();
  Bluetooth_Self_Check();
  OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    // /* USER CODE BEGIN 3 */
    if(run_mode == 0)
    {
      HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_RESET);//输出低
      if(get_mode == 2)
      {
        tar_direct = angle_z_all;
        run_mode = 2;
      }        
      else if(get_mode == 3)
      {
        tar_direct = angle_z_all;
        run_mode = 3;
      }
      else if(get_mode == 4)
      {
        tar_direct = angle_z_all;
        run_mode = 4;
      }
      else if(get_mode == 5)
      {
        tar_direct = angle_z_all;
        run_mode = 5;
      }
      else if(get_mode == 6)
      {
        tar_direct = angle_z_all;
        run_mode = 6;
      }
      else if(get_mode == 7)
      {
        tar_direct = angle_z_all;
        run_mode = 7;
      }
      OLED_ShowString(0,0,(unsigned char *)"choosing",16);
      OLED_Refresh();
    }
    if(run_mode != 0)
    {
      HAL_GPIO_WritePin(LIFE_LED_GPIO_PORT,LIFE_LED_GPIO_PIN,GPIO_PIN_SET);//输出低
      OLED_ShowString(0,0,(unsigned char *)"running",16);
      OLED_ShowString(0,16,(unsigned char *)"mode",16);
      OLED_ShowNum(0,32,run_mode,1,16);
      OLED_Refresh();
      get_mode = 0;
    }
    switch(run_mode)
    {
      case 0:
        break;
      case 2://一个点
        run_one(tar_point[0],tar_point[1]);
        break;
      case 3://一个点且返回
        run_three(tar_point[0],tar_point[1]);
        break;
      case 4://两个点
        run_four(tar_point[0],tar_point[1],tar_point[2],tar_point[3]);
        break;
      case 5:
        run_five();
        break;
      case 6:
        run_six();
        break;
      case 7:
        run_seven();
        break;   
      default:
        break;  
    }


    
    if(recv_end_flag == 1)  
		{
      if(rx_buffer[0] == 85 && rx_buffer[1] == 83)
      {
        angle_z = (float)((rx_buffer[7]<<8)|rx_buffer[6])/32768.0*180.0;
      }
      if(init_count<3 && angle_z_temp != 0)
      {
          JY901_Init_val[init_count++] = angle_z_temp;
      }
      else if(init_count == 3)
      {
          JY901_Init_val[init_count++] = (JY901_Init_val[0]+JY901_Init_val[1]+JY901_Init_val[2])/3;
      }
      if(angle_z > 300 && angle_z_last < 50) jy901_of_count--;
      else if(angle_z < 50 && angle_z_last > 300) jy901_of_count++;
      angle_z_all = angle_z + jy901_of_count * 360;
      angle_z_last = angle_z;//保存上一次的值
      angle_z_temp = angle_z_all;
			memset(rx_buffer, 0, rx_len);
    	rx_len = 0;
			recv_end_flag = 0;
			HAL_Delay(1);
			HAL_UART_Receive_DMA(&huart3, rx_buffer, BUFFER_SIZE);
		}
  }
    
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/******************************
 * 功能：进行各模块的初始化
 * 输入：无
 * 返回：无
 * ******************************/
void Car_All_Init(void)
{
  Motor_Init();//电机相关结构体与定时器初始化
  PID_Init();//PID参数初始�??????
  Bluetooth_Init();//蓝牙初始�??????
  OLED_Init_Run();//OLED初始�??????
  //JY901_Init();//JY901初始�?????
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  // while (1)
  // {
  // }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
