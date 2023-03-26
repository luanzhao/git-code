/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_hal.h"
//变量创建区
__IO uint32_t uwTick_Key_Set_Point = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Lcd_Set_Point = 0;//控制Lcd_Proc的执行速度
__IO uint32_t uwTick_Usart_Set_Point = 0;//控制Usart_Proc的执行速度
unsigned char i;

//*按键扫描专用变量
unsigned char ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;

//*LCD显示专用变量
unsigned char Lcd_Disp_String[21];//最多显示20个字符

//*串口专用变量
int counter = 0;
char str[40];
unsigned char rx_buffer;

//*EEPROM的相关变量
unsigned char EEPROM_String_1[5] = {0,2,4,6,8};
unsigned char EEPROM_String_2[5] = {0};

//*4017相关变量
uint8_t RES_4017;


//子函数声明区

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Key_Proc(void);
void Lcd_Proc(void);
void Usart_Proc(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
	I2CInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	//**EEPROM测试
	iic_24c02_write(EEPROM_String_1, 0, 5);
	HAL_Delay(1);	
	iic_24c02_read(EEPROM_String_2, 0, 5);

  //**MCP4017测试
	write_resistor(0x77);
	RES_4017 = read_resistor();
	HAL_TIM_Base_Start_IT(&htim6);
//	HAL_ADC_Start_IT(&hadc1);
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		Key_Proc();
		Lcd_Proc();
		Usart_Proc();	
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Key_Proc(void)
{
	if((uwTick -  uwTick_Key_Set_Point)<100)	return;//减速函数
	uwTick_Key_Set_Point = uwTick;
	
	ucKey_Val = Key_Scan();
	unKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val); 
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);	
	ucKey_Old = ucKey_Val;
	
	switch(unKey_Down)
	{
		case 4:
			LED_Disp(0x88);
		break;
		case 3:
			LED_Disp(0x00);
		break;
	}
	if(((((float)getADC2())/4096)*3.3)<3.0)
	{
		LED_Disp(0x77);
	}
	else
		LED_Disp(0x00);
}

void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<300)	return;//减速函数
	uwTick_Lcd_Set_Point = uwTick;
//	i++;
	
	sprintf((int8_t *)Lcd_Disp_String, "   i num: %03d        ",(unsigned int)i);
	LCD_DisplayStringLine(Line1, Lcd_Disp_String);
	sprintf((char *)Lcd_Disp_String, "EE:%x%x%x%x%x R:%x",EEPROM_String_2[0],EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4], RES_4017);
	LCD_DisplayStringLine(Line2, Lcd_Disp_String);
	sprintf((char *)Lcd_Disp_String, "RES_K:%5.2fK",0.7874*RES_4017);
	LCD_DisplayStringLine(Line3, Lcd_Disp_String);	
	sprintf((char *)Lcd_Disp_String, "VOLTAGE:%6.3fV",3.3*((0.7874*RES_4017)/(0.7874*RES_4017+10)));
	LCD_DisplayStringLine(Line4, Lcd_Disp_String);	
	
	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",((((float)getADC1())/4096)*3.3));
	LCD_DisplayStringLine(Line6, Lcd_Disp_String);	
	
	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",((((float)getADC2())/4096)*3.3));
	LCD_DisplayStringLine(Line7, Lcd_Disp_String);	
	
}

void Usart_Proc(void)
{
	if((uwTick -  uwTick_Usart_Set_Point)<500)	return;//减速函数
	uwTick_Usart_Set_Point = uwTick;
	
	sprintf(str, "%04d:Hello,world.\r\n", counter);
	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);
	
	if(++counter == 10000)
		counter = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	LED_Disp(0xff);
	HAL_Delay(300);
	LED_Disp(0x00);	
	
	HAL_UART_Receive_IT(&huart1, &rx_buffer, 1);
	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	i++;
	HAL_TIM_Base_Start_IT(&htim6);
}

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
//{
//	if(((((float)getADC2())/4096)*3.3)<3.0)
//	{
//		LED_Disp(0x77);
//	}
//	else
//		LED_Disp(0x00);
//	HAL_ADC_Start_IT(&hadc1);
//	
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
