/*
 * alarm.c
 *
 *  Created on: Oct 25, 2020
 *      Author: michal
 */

#include "rtc.h"
#include "time.h"
#include "lcd_i2c.h"
#include "usart.h"
#include <string.h>

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

uint8_t global_counter = 0;
uint8_t global_buffer[5];

/*	time	*/
uint8_t time_activated_flag;
uint8_t time_set_flag;
/*	time	*/

/*	date	*/
uint8_t date_activated_flag;
uint8_t date_set_flag;
/*	date	*/

/*	alarm	*/
uint8_t alarm_activated_flag;
uint8_t alarm_set_flag;
uint8_t alarm_flag;

char alarm_on_msg[20] = "Wylacz alarm!\n\r";
char alarm_off_msg[20] = "Alarm wylaczony!\n\r";
char alarm_set_msg[20] = "Alarm ustawiony!\n\r";

uint8_t add_sec, add_mins, add_hours, add_days;
uint8_t temp_sec, temp_mins, temp_hours, temp_days;
/*	alarm	*/

uint8_t uart_rx_data;

void rtc_set_time ()
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	get_Time();

	uint8_t hour = time.hour;
	uint8_t min = time.minutes;
	uint8_t sec = time.seconds;

	uint8_t day = time.dayofmonth;
	uint8_t year = time.year;

	// czas w RTC będzie taki sam jak w naszym DS3231
	sTime.Hours = decToBcd(hour);
	sTime.Minutes = decToBcd(min);
	sTime.Seconds = decToBcd(sec);

	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
	  Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 3 */

	/* USER CODE END RTC_Init 3 */

	switch( (int) time.dayofweek) {

	case 1:
		sDate.WeekDay = RTC_WEEKDAY_MONDAY;
		break;

	case 2:
		sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
		break;

	case 3:
		sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
		break;

	case 4:
		sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
		break;

	case 5:
		sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
		break;

	case 6:
		sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
		break;

	case 7:
		sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
		break;

	}


	switch( (int) time.month) {

	  case 1:
		  sDate.Month = RTC_MONTH_JANUARY;
		  break;

	  case 2:
		  sDate.Month = RTC_MONTH_FEBRUARY;
		  break;

	  case 3:
		  sDate.Month = RTC_MONTH_MARCH;
		  break;

	  case 4:
		  sDate.Month = RTC_MONTH_APRIL;
		  break;

	  case 5:
		  sDate.Month = RTC_MONTH_MAY;
		  break;

	  case 6:
		  sDate.Month = RTC_MONTH_JUNE;
		  break;

	  case 7:
		  sDate.Month = RTC_MONTH_JULY;
		  break;

	  case 8:
		  sDate.Month = RTC_MONTH_AUGUST;
		  break;

	  case 9:
		  sDate.Month = RTC_MONTH_SEPTEMBER;
		  break;

	  case 10:
		  sDate.Month = RTC_MONTH_OCTOBER;
		  break;

	  case 11:
		  sDate.Month = RTC_MONTH_NOVEMBER;
		  break;

	  case 12:
		  sDate.Month = RTC_MONTH_DECEMBER;
		  break;

	 }

	sDate.Date = decToBcd(day);
	sDate.Year = decToBcd(year);


	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 4 */

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);  // backup register

	/* USER CODE END RTC_Init 4 */

}

// Funkcja odpowiedzialna za ustawienie alarmu o danej godzinie, i w danym dniu.
// Jako parametry przyjmuje ilość godzin, minut, sekund oraz dni, pozostałych do włączenia alarmu
void rtc_set_alarm (uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	get_Time();

	temp_sec = time.seconds + sec;
	temp_mins = time.minutes + min;
	temp_hours = time.hour + hour;
	temp_days = time.dayofmonth + day;

	if(temp_sec > 59) {

		add_sec = temp_sec % 60;
		add_mins = temp_sec / 60;

		temp_sec = add_sec;
		temp_mins += add_mins;

		if(temp_mins > 59) {

			add_mins = temp_mins % 60;
			add_hours = temp_mins / 60;

			temp_mins = add_mins;
			temp_hours += add_hours;

			if(temp_hours > 23) {

				add_hours = temp_hours % 24;
				add_days = temp_hours / 24;

				temp_hours = add_hours;
				temp_days += add_days;
			}
		}
	}

	uint8_t alarm_day = temp_days;
	uint8_t alarm_hour = temp_hours;
	uint8_t alarm_min = temp_mins;

	// włączamy alarm 2 sekundy wcześniej niż zaplanowany, ponieważ transmisja uartem trwa 2s
	uint8_t alarm_sec = temp_sec - 2;

	RTC_AlarmTypeDef sAlarm;

    /**Enable the Alarm A
    */
	sAlarm.AlarmTime.Hours = decToBcd(alarm_hour);
	sAlarm.AlarmTime.Minutes = decToBcd(alarm_min);
	sAlarm.AlarmTime.Seconds = decToBcd(alarm_sec);
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = decToBcd(alarm_day);
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_UART_Transmit_IT(&huart2, (uint8_t *)alarm_set_msg, strlen(alarm_set_msg));

  /* USER CODE BEGIN RTC_Init 5 */

  /* USER CODE END RTC_Init 5 */
}

// Funkcja sygnalizująca, że użytkownik ustawia nową godzinę na zegarku
void activate_time () {

	if(uart_rx_data == 't') {

		date_activated_flag = 0;
		alarm_activated_flag = 0;

		global_counter = 0;
		time_activated_flag = 1;
	}

	if(time_activated_flag) {

		global_buffer[global_counter] = uart_rx_data;

		++global_counter;

		if(global_counter == 3) {
			time_set_flag = 1;
		}

	}

	// Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
}

// Funkcja sygnalizująca, że użytkownik ustawia nową datę na zegarku
void activate_date () {

	if(uart_rx_data == 'd') {

		time_activated_flag = 0;
		alarm_activated_flag = 0;

		global_counter = 0;
		date_activated_flag = 1;
	}

	if(date_activated_flag) {

		global_buffer[global_counter] = uart_rx_data;

		++global_counter;

		if(global_counter == 5) {
			date_set_flag = 1;
		}

	}

	// Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
}

// Funkcja sygnalizująca, że użytkownik ustawia nowy alarm
void activate_alarm () {

	if(uart_rx_data == 'a') {

		time_activated_flag = 0;
		date_activated_flag = 0;

		global_counter = 0;
		alarm_activated_flag = 1;
	}

	if(alarm_activated_flag) {

		global_buffer[global_counter] = uart_rx_data;

		++global_counter;

		if(global_counter == 5) {
			alarm_set_flag = 1;
		}

	}

	// Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
}



void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	alarm_flag = 1;
}

void to_do_on_alarm() {

	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit_IT(&huart2, (uint8_t *)alarm_on_msg, strlen(alarm_on_msg));
	lcd_send_alarm_on_msg();
}

void to_do_on_alarm_off() {

	HAL_UART_Transmit_IT(&huart2, (uint8_t *)alarm_off_msg, strlen(alarm_off_msg));
	lcd_send_alarm_off_msg();
}

// Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	// Sprawdzamy czy przerwanie wywołał niebieski przycisk
	if(GPIO_Pin == Blue_Button_Pin) {

		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
		alarm_flag = 0;
	}
}
