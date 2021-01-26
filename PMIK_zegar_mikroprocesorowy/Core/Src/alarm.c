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

/**
 * Struktura do przechowywania czasu i daty, które będziemy odczytywać z DS3231
 */
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

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

uint8_t global_counter = 0;
uint8_t global_buffer[5];

/*	time	*/
uint8_t time_activated_flag;
uint8_t time_set_flag;
uint8_t hour, min, sec;
/*	time	*/

/*	date	*/
uint8_t date_activated_flag;
uint8_t date_set_flag;
uint8_t day, year;
/*	date	*/

/*	alarm	*/
RTC_AlarmTypeDef sAlarm;
uint8_t alarm_day, alarm_hour, alarm_min, alarm_sec;

uint8_t alarm_activated_flag;
uint8_t alarm_set_flag;
uint8_t alarm_flag;

char alarm_on_msg[20] = "Wylacz alarm!\n\r";
char alarm_off_msg[20] = "Alarm wylaczony!\n\r";

char alarm_details_msg[9];
/*	alarm	*/

/* UART */
uint8_t uart_rx_data, BT_rx_data;
/* UART */


void rtc_set_time ()
{
	get_Time();

	hour = time.hour;
	min = time.minutes;
	sec = time.seconds;

	day = time.dayofmonth;
	year = time.year;

	/**
	 * czas w RTC będzie taki sam jak w naszym DS3231
	 */
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

/**
 * Funkcja odpowiedzialna za ustawienie alarmu o danej godzinie, i w danym dniu.
 * Jako parametry przyjmuje ilość dni do alarmu, godzinę, minutę oraz sekundę alarmu
 */
void rtc_set_alarm (uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
	get_Time();

	if( (day >= 0) && (hour >= 0) && (hour < 24) && (min >= 0) && (min < 60) && (sec >= 0) && (sec < 60) ) {

		if(hour > time.hour) {

			alarm_day = (time.dayofmonth + day) % 31;
			alarm_hour = hour;
			alarm_min = min;
			alarm_sec = sec;

			/**Enable the Alarm A*/
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

			lcd_clear();
			sprintf(alarm_details_msg, "%02d:%02d:%02d", hour, min, sec);
			lcd_send_string("Alarm na godz.:");
			lcd_second_line();
			lcd_send_string(alarm_details_msg);
			delay(1000);
			lcd_clear();

		}
		else if ( (hour == time.hour) ) {

			if( (min > time.minutes) ) {

				alarm_day = time.dayofmonth + (day % 7);
				alarm_hour = hour;
				alarm_min = min;
				alarm_sec = sec;

				/**Enable the Alarm A*/
				sAlarm.AlarmTime.Hours = decToBcd(alarm_hour);
				sAlarm.AlarmTime.Minutes = decToBcd(alarm_min);
				sAlarm.AlarmTime.Seconds = decToBcd(alarm_sec - 2);
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

				lcd_clear();
				sprintf(alarm_details_msg, "%02d:%02d:%02d", hour, min, sec);
				lcd_send_string("Alarm na godz.:");
				lcd_second_line();
				lcd_send_string(alarm_details_msg);
				delay(1000);
				lcd_clear();
			}
			else if ( (min == time.minutes) ) {

				if( (sec > time.seconds) ) {

					alarm_day = time.dayofmonth + (day % 7);
					alarm_hour = hour;
					alarm_min = min;
					alarm_sec = sec;

					/**Enable the Alarm A*/
					sAlarm.AlarmTime.Hours = decToBcd(alarm_hour);
					sAlarm.AlarmTime.Minutes = decToBcd(alarm_min);
					sAlarm.AlarmTime.Seconds = decToBcd(alarm_sec - 2);
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

					lcd_clear();
					sprintf(alarm_details_msg, "%02d:%02d:%02d", hour, min, sec);
					lcd_send_string("Alarm na godz.:");
					lcd_second_line();
					lcd_send_string(alarm_details_msg);
					delay(1000);
					lcd_clear();
				}
				else {
					lcd_clear();
					lcd_set_cursor(0, 1);
					lcd_send_string("Nieprawid");
					lcd_send_own_char(3);
					lcd_send_string("owy");
					lcd_second_line();
					lcd_set_cursor(1, 2);
					lcd_send_string("czas alarmu!");
					delay(1000);
					lcd_clear();
				}

			}
			else {

			}
		}
		else {
			lcd_clear();
			lcd_set_cursor(0, 1);
			lcd_send_string("Nieprawid");
			lcd_send_own_char(3);
			lcd_send_string("owy");
			lcd_second_line();
			lcd_set_cursor(1, 2);
			lcd_send_string("czas alarmu!");
			delay(1000);
			lcd_clear();
		}

	} else {
		lcd_clear();
		lcd_set_cursor(0, 1);
		lcd_send_string("Nieprawid");
		lcd_send_own_char(3);
		lcd_send_string("owy");
		lcd_second_line();
		lcd_set_cursor(1, 2);
		lcd_send_string("czas alarmu!");
		delay(1000);
		lcd_clear();
	}


  /* USER CODE BEGIN RTC_Init 5 */

  /* USER CODE END RTC_Init 5 */
}

/**
 * Funkcja sygnalizująca, że użytkownik ustawia nową godzinę na zegarku
 */
void activate_time () {

	if(uart_rx_data == 't' || uart_rx_data == 'T') {

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

	/**
	 * Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	 */
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
	HAL_UART_Receive_IT(&huart6, &BT_rx_data, 1);
}

/**
 * Funkcja sygnalizująca, że użytkownik ustawia nową datę na zegarku
 */
void activate_date () {

	if(uart_rx_data == 'd' || uart_rx_data == 'D') {

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

	/**
	 * Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	 */
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
	HAL_UART_Receive_IT(&huart6, &BT_rx_data, 1);
}

/**
 * Funkcja sygnalizująca, że użytkownik ustawia nowy alarm
 */
void activate_alarm () {

	if(uart_rx_data == 'a' || uart_rx_data == 'A') {

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

	/**
	 * Po odebraniu danych, nasłuchuj ponownie na kolejne znaki
	 */
	HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
	HAL_UART_Receive_IT(&huart6, &BT_rx_data, 1);
}



void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	alarm_flag = 1;
}

void to_do_on_alarm() {

	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
	lcd_send_alarm_on_msg();
}

void to_do_on_alarm_off() {

	lcd_send_alarm_off_msg();
}

/**
 * Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	/**
	 * Sprawdzamy czy przerwanie wywołał niebieski przycisk
	 */
	if(GPIO_Pin == Blue_Button_Pin) {

		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
		alarm_flag = 0;
	}
}
