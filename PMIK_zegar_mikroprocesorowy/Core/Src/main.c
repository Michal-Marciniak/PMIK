/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "lcd_i2c.h"

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

uint8_t uart_rx_data;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define DS3231_ADDRESS 0xD0

// Funkcja konwertująca wartość dziesiętną na binarną,
// ponieważ dane zapisywane do rejestrów DS3231 muszą być postaci binarnej
uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}

// Funkcja konwertująca wartość binarną na decymalną,
// ponieważ dane odczytywane z DS3231 są postaci binarnej, a dane wyświetlane na LCD będą postaci dziesiętnej
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

// Struktura do przechowywania czasu i daty, które będziemy odczytywać z DS3231
typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} TIME;

TIME time;

// Funkcje do ustawiania i pobierania czasu i daty
// Przekazywane wartości muszą być typu binarnego
void set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t time_to_write[7];

	time_to_write[0] = decToBcd(sec);
	time_to_write[1] = decToBcd(min);
	time_to_write[2] = decToBcd(hour);
	time_to_write[3] = decToBcd(dow);
	time_to_write[4] = decToBcd(dom);
	time_to_write[5] = decToBcd(month);
	time_to_write[6] = decToBcd(year);

	HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, time_to_write, 7, 1000);
}

void get_Time (void)
{
	uint8_t time_to_red[7];

	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, time_to_red, 7, 1000);

	time.seconds = bcdToDec(time_to_red[0]);
	time.minutes = bcdToDec(time_to_red[1]);
	time.hour = bcdToDec(time_to_red[2]);
	time.dayofweek = bcdToDec(time_to_red[3]);
	time.dayofmonth = bcdToDec(time_to_red[4]);
	time.month = bcdToDec(time_to_red[5]);
	time.year = bcdToDec(time_to_red[6]);
}

char time_date_buffer[11];

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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Funkcja odpowiedzialna za odbiór jednego znaku z uart2 w trybie przerwaniowym, i zapisanie go w zmiennej uart_rx_data.
  // Po odebraniu znaku, nastąpi przerwanie które zostanie obsłużone przez funkcję callback.
  HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);

  lcd_init();

  // Tutaj domyślnie będzie pin z brzęczykiem
  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_RESET);

  //set_Time(30, 11, 23, 6, 24, 10, 20);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	get_Time();

	sprintf(time_date_buffer, "%02d:%02d:%02d", time.hour, time.minutes, time.seconds);
	lcd_first_line();
	lcd_send_string(time_date_buffer);

	// wyświetl nazwę dnia tygodnia
	lcd_set_cursor(0, 10);
	lcd_show_week_day_name(time.dayofweek);

	sprintf(time_date_buffer, "%02d-%02d-20%02d", time.dayofmonth, time.month, time.year);
	lcd_second_line();
	lcd_send_string(time_date_buffer);

	HAL_Delay(1000);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
}

/* USER CODE BEGIN 4 */

// Funkcja odpowiedzialna za obsługę przerwania spowodowanego odebraniem danych na UART2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	// Musimy sprawdzić czy przerwanie wywołał uart2, a nie coś innego
	if(huart->Instance == USART2) {

		// Tutaj domyślnie będzie obsługa wyłączenia alarmu
		if(uart_rx_data == 'd') {
			HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_SET);
		}

		// Po obsłużeniu przerwania, znowu nasłuchujemy czy nie przyszedł kolejny znak z uarta
		HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
	}
}

// Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	// Sprawdzamy czy przerwanie wywołał niebieski przycisk
	if(GPIO_Pin == Blue_Button_Pin) {
		HAL_GPIO_TogglePin(Green_LED_GPIO_Port, Green_LED_Pin);
	}
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
