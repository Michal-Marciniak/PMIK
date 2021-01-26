/* USER CODE BEGIN Header */

/** Projekt zegara mikroprocesorowego zrealizowanego na płytce Nucleo F401RE.
 * Układ posiada możliwość wyświetlania czasu, daty oraz dnia tygodnia na wyświetlaczu LCD HD44780.
 * Wyświetlacz podłączony jest do płytki za pomocą interfejsu I2C.
 * Dodatkowo układ posiada moduł RTC czasu rzeczywistego,
 * dzięki któremu możliwe jest zapamiętanie aktualnego czasu i daty, nawet po wyłączeniu zasilania mikroprocesora.
 * Za pomocą klawiatury w komputerze, użtykownik jest w stanie zmienić czas, datę lub nastawić alarm.
 * Komunikacja ta odbywa się za pomocą intefejsu USART.
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include "lcd_i2c.h"
#include "time.h"
#include "alarm.h"
#include "standby.h"
#include <stdbool.h>
#include <stdbool.h>

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t uart_rx_data;
uint8_t BT_rx_data;

uint8_t i, is_BT_rx_complete;
char BT_rx_string[2];
int BT_rx_int, BT_array_max_size, is_BT_data_valid;

uint8_t alarm_flag;
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
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_RESET);

  /****************** LCD BEGIN ******************/
  lcd_init();
  /****************** LCD END ******************/

  /**
   * Wpisanie do rejestru RTC, czasu i daty pobranych z modułu RTC, aby czas w RTC był aktualny
   */
  rtc_set_time();

  /****************** UART BEGIN ******************/
  /**
   * Funkcja odpowiedzialna za odbiór jednego znaku z uart2 w trybie przerwaniowym, i zapisanie go w zmiennej uart_rx_data.
   * Po odebraniu znaku, nastąpi przerwanie które zostanie obsłużone przez funkcję callback HAL_UART_RxCpltCallback.
   */
  HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
  HAL_UART_Receive_IT(&huart6, &BT_rx_data, 1);
  /****************** UART END ******************/

  /****************** TIMER BEGIN ******************/
  HAL_TIM_Base_Start_IT(&htim1);
  /****************** TIMER END ******************/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		to_do_after_wake_up_from_standby();

		to_do_before_going_to_standby();

		go_to_standby();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 60;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * Funkcja odpowiedzialna za obsługę przerwania spowodowanego odebraniem danych na UART2
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	/**
	 * Musimy sprawdzić czy przerwanie wywołał uart2, a nie coś innego
	 */
	if (huart->Instance == USART2) {

		activate_time();
		activate_date();
		activate_alarm();

		/**
		 * Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
		 */
		HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
	}
	else if (huart->Instance == USART6) {

		is_BT_rx_complete = 0;

		if( (BT_rx_data == 'e') && (BT_array_max_size > 0 && BT_array_max_size <= 2) && (is_BT_data_valid == 1)) {

			if(BT_array_max_size == 1) {

				uart_rx_data = BT_rx_string[0];
			}
			else if(BT_array_max_size == 2) {

				uart_rx_data = BT_rx_int;
			}

			i = 0;
			BT_array_max_size = 0;
			is_BT_data_valid = 0;
			is_BT_rx_complete = 1;
		}
		else if (BT_rx_data == 'q') {

			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			alarm_flag = 0;
		}
		else {

			if(BT_rx_data == 't' || BT_rx_data == 'd' || BT_rx_data == 'a' || BT_rx_data == 'T' || BT_rx_data == 'D' || BT_rx_data == 'A') {

				BT_rx_string[0] = BT_rx_data;

				BT_array_max_size = 1;
				is_BT_data_valid = 1;
			}
			else if(BT_rx_data != 'e' && BT_rx_data >= 0) {

				BT_rx_string[i] = BT_rx_data - 48;

				i = i + 1;
				BT_array_max_size = i;

				if(i == 2) {

					char temp_string[2];
					char left_part[2], right_part[2];

					sprintf(left_part, "%d", BT_rx_string[0]);	/** np. 1 */
					sprintf(right_part, "%d", BT_rx_string[1]);	/** np. 2 */
					strcat(left_part, right_part);	/** dołączenie right_part na koniec left_part (left_part = 12) */

					strcpy(temp_string, left_part);	/** przekopiowanie left_part do temp_string. (temp_string = 12) */

					BT_rx_int = atoi(temp_string);	/** BT_rx_int = 12 */

					i = 0;
				}

				is_BT_data_valid = 1;
			}
		}

		if(is_BT_rx_complete == 1) {

			activate_time();
			activate_date();
			activate_alarm();
		}

		/**
		 * Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
		 */
		HAL_UART_Receive_IT(&huart6, &BT_rx_data, 1);
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
