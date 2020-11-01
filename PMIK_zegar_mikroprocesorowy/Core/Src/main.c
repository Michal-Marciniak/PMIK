/* USER CODE BEGIN Header */

/* Projekt zegara mikroprocesorowego zrealizowanego na płytce Nucleo F401RE.
 * Układ posiada możliwość wyświetlania czasu, daty lub alarmów na wyświetlaczu LCD HD44780,
 * podłączonym do płytki za pomocą interfejsu i2c.
 * Dodatkowo układ posiada moduł RTC czasu rzeczywistego, dzięki któremu możliwe jest
 * zapamiętanie aktualnego czasu i daty, nawet po wyłączeniu zasilania mikroprocesora.
 * Za pomocą klawiatury w komputerze, użtykownik jest w stanie zmienić czas, datę lub nastawić alarm.
 * Komunikacja ta odbywa się za pomocą intefejsu USART.
 */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include "lcd_i2c.h"
#include "time.h"
#include "alarm.h"

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

// Struktura do przechowywania aktualnego czasu i daty, które będziemy odczytywać z modułu RTC DS3231
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

uint8_t global_counter;			// globalny licznik, służący do ustawiania nowego czasu, daty i alarmu ustawionych przez użytkownika
uint8_t global_buffer[5];		// globalny bufor przechowujący nowy czas, datę i alarm ustawione przez użytkownika

/*	time	*/
uint8_t time_activated_flag;	// flaga sygnalizująca, że w tym momencie użytkownik ustawia nowy czas
uint8_t time_set_flag;			// flaga sygnalizująca, że nowy czas został pomyślnie ustawiony
/*	time	*/

/*	date	*/
uint8_t date_activated_flag;	// flaga sygnalizująca, że w tym momencie użytkownik ustawia nową date
uint8_t date_set_flag;			// flaga sygnalizująca, że nowa data została pomyślnie ustawiona
/*	date	*/

/*	alarm	*/
uint8_t alarm_activated_flag;	// flaga sygnalizująca, że w tym momencie użytkownik ustawia nowy alarm
uint8_t alarm_set_flag; 		// flaga sygnalizująca, że alarm został pomyślnie ustawiony
uint8_t alarm_flag; 			// flaga sygnalizująca włączenie się alarmu
/*	alarm	*/

/*	Uart received data	*/
uint8_t uart_rx_data;
/*	Uart received data	*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_RESET);

  /****************** LCD BEGIN ******************/
  lcd_init();
  lcd_back_light_on();
  /****************** LCD END ******************/

  rtc_set_time();	// Wpisanie do rejestru RTC, czasu i daty pobranych z DS3231, aby czas w RTC był aktualny

  /****************** UART BEGIN ******************/
  // Funkcja odpowiedzialna za odbiór jednego znaku z uart2 w trybie przerwaniowym, i zapisanie go w zmiennej uart_rx_data.
  // Po odebraniu znaku, nastąpi przerwanie które zostanie obsłużone przez funkcję callback HAL_UART_RxCpltCallback.
  HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);

  // wystąpienie przerwania po ukończeniu odbioru informacji (RXNE - Register Not Empty)
  //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

  // wystąpienie przerwania po ukończeniu transmisji danych (TC - Transmition Complete)
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
  /****************** UART END ******************/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	lcd_display_refresh();

	// wykonaj jeśli użytkownik ustawił nową godzinę
	if(time_set_flag) {

		uint8_t new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year;
		char new_time_details_msg[12];

		new_sec = time.seconds;
		new_min = global_buffer[2];
		new_hour = global_buffer[1];
		new_dow = time.dayofweek;
		new_dom = time.dayofmonth;
		new_month = time.month;
		new_year = time.year;

		set_Time(new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year);

		lcd_clear();
		sprintf(new_time_details_msg, "%02d:%02d:%02d", new_hour, new_min, new_sec);
		lcd_send_string("Ustawiony czas:");
		lcd_second_line();
		lcd_send_string(new_time_details_msg);
		HAL_Delay(3000);
		lcd_clear();

		time_set_flag = 0;
	}

	// wykonaj jeśli użytkownik ustawił nową datę
	if(date_set_flag) {

		uint8_t new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year;
		char new_date_details_msg[12];

		new_sec = time.seconds;
		new_min = time.minutes;
		new_hour = time.hour;
		new_dow = global_buffer[1];
		new_dom = global_buffer[2];
		new_month = global_buffer[3];
		new_year = global_buffer[4];

		set_Time(new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year);

		lcd_clear();
		sprintf(new_date_details_msg, "%02d-%02d-20%02d", new_dom, new_month, new_year);
		lcd_send_string("Ustawiona data:");
		lcd_second_line();
		lcd_send_string(new_date_details_msg);
		HAL_Delay(3000);
		lcd_clear();

		date_set_flag = 0;
	}

	// wykonaj jeśli użytkownik ustawił nowy czas alarmu
	if(alarm_set_flag) {

		uint8_t days_to_alarm, hours_to_alarm, minutes_to_alarm, seconds_to_alarm;
		char alarm_details_msg[9];

		days_to_alarm = global_buffer[1];	// dni to drugi element, ponieważ pierwszy to literka a
											// mówiąca o tym, że ustawiamy alarm.
		hours_to_alarm = global_buffer[2];
		minutes_to_alarm = global_buffer[3];
		seconds_to_alarm = global_buffer[4];

		rtc_set_alarm(days_to_alarm, hours_to_alarm, minutes_to_alarm, seconds_to_alarm);

		lcd_clear();

		sprintf(alarm_details_msg, "%02d:%02d:%02d", time.hour + hours_to_alarm, time.minutes + minutes_to_alarm, time.seconds + seconds_to_alarm);
		lcd_send_string("Alarm na godz.:");
		lcd_second_line();
		lcd_send_string(alarm_details_msg);
		HAL_Delay(3000);
		lcd_clear();

		alarm_set_flag = 0;
	}

	// wykonaj jeśli zaczął się alarm
	if(alarm_flag) {

		while (alarm_flag) {
			to_do_on_alarm();
		}

		to_do_on_alarm_off();
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
  RCC_OscInitStruct.PLL.PLLN = 64;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

// Funkcja odpowiedzialna za obsługę przerwania spowodowanego odebraniem danych na UART2
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	// Musimy sprawdzić czy przerwanie wywołał uart1, a nie coś innego
	if(huart->Instance==USART2)
	{
		activate_time();
		activate_date();
		activate_alarm();

		// Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
		HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
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
