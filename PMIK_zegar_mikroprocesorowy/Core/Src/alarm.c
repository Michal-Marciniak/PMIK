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

uint8_t alarm_flag;

char alarm_on_msg[20] = "Wylacz alarm!\n\r";
char alarm_off_msg[20] = "Alarm wylaczony!\n\r";

void rtc_set_time (void)
{
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;
	  /**Initialize RTC and set the Time and Date
	  */

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

	uint8_t alarm_day = time.dayofmonth + day;
	uint8_t alarm_hour = time.hour + hour;
	uint8_t alarm_min = time.minutes + min;
	uint8_t alarm_sec = time.seconds + sec;

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
  /* USER CODE BEGIN RTC_Init 5 */

  /* USER CODE END RTC_Init 5 */
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	alarm_flag = 1;
}

void to_do_on_alarm(void) {

	lcd_clear();
	//lcd_back_light_on();

	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit_IT(&huart2, (uint8_t *)alarm_on_msg, 20);
	lcd_send_alarm_on_msg();

	HAL_Delay(1000);
}

void to_do_on_alarm_off(void) {

	lcd_clear();
	//lcd_back_light_on();

	HAL_UART_Transmit_IT(&huart2, (uint8_t *)alarm_off_msg, 20);
	lcd_send_alarm_off_msg();
	HAL_Delay(3000);

	lcd_clear();
}

// Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	// Sprawdzamy czy przerwanie wywołał niebieski przycisk
	if(GPIO_Pin == Blue_Button_Pin) {

		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
		alarm_flag = 0;
	}
}
