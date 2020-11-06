/*
 * standby_init.c
 *
 *  Created on: Nov 4, 2020
 *      Author: michal
 */

#include "rtc.h"
#include "lcd_i2c.h"
#include "time.h"
#include "alarm.h"
#include "main.h"


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

uint8_t add_sec, add_mins, add_hours, add_days;
uint8_t temp_sec, temp_mins, temp_hours, temp_days;
/*	alarm	*/


void to_do_after_wake_up_from_standby(void) {

	// wykonaj jeśli zaczął się alarm
	if(alarm_flag) {

		while (alarm_flag) {
			to_do_on_alarm();
		}

		to_do_on_alarm_off();
	}

	// Flaga SBF (Standby flag) mówi o tym, czy MCU wybudził się ze stanu standby
	// SBF = 1 -> MCU wybudził się ze standby

	// na początku sprawdzamy czy nie jest ustawiona flaga SBF
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {

	  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);	// wyszyszczenie flagi SBF, aby upewnić się że nie jesteśmy w trybie stanby

	  for(int i=0; i<10; i++) {
		  HAL_GPIO_TogglePin(Green_LED_GPIO_Port, Green_LED_Pin);
		  HAL_Delay(100);
	  }

	  if(!alarm_flag) {
		  lcd_clear();
		  lcd_set_cursor(0, 2);
		  lcd_send_string("Wake up from");
		  lcd_second_line();
		  lcd_set_cursor(1, 2);
		  lcd_send_string("STANDBY MODE");
		  HAL_Delay(1500);
		  lcd_clear();
	  } else {
		  to_do_on_alarm();
	  }

	  // Wyłączenie pinu WAKE UP
	  HAL_PWR_DisableWakeUpPin(WakeUp_PIN_Pin);

	  // Wyłączenie Wake up timer
	  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}

}

void to_do_before_going_to_standby(void) {

	// wykonaj jeśli zaczął się alarm
	if(alarm_flag) {

		while (alarm_flag) {
			to_do_on_alarm();
		}

		to_do_on_alarm_off();
	}

	for(int i=0; i<30; i++) {

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

			if( (new_hour > 23 || new_hour < 0 || new_min > 59 || new_min < 1) && time_set_flag) {
				lcd_clear();
				lcd_set_cursor(0, 2);
				lcd_send_string("B");
				lcd_send_own_char(3);
				lcd_send_own_char(2);
				lcd_send_string("dny czas!");
				HAL_Delay(1000);
				lcd_clear();

				time_set_flag = 0;
			}
			else {
				set_Time(new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year);

				lcd_clear();
				sprintf(new_time_details_msg, "%02d:%02d:%02d", new_hour, new_min, new_sec);
				lcd_send_string("Ustawiony czas:");
				lcd_second_line();
				lcd_send_string(new_time_details_msg);
				HAL_Delay(1000);
				lcd_clear();

				time_set_flag = 0;
			}

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

			if( (new_dow > 7 || new_dow < 1 || new_dom > 31 || new_dom < 1 || new_month > 12 || new_month < 1) && date_set_flag) {
				lcd_clear();
				lcd_set_cursor(0, 2);
				lcd_send_string("B");
				lcd_send_own_char(3);
				lcd_send_own_char(2);
				lcd_send_string("dna data!");
				HAL_Delay(1000);
				lcd_clear();

				date_set_flag = 0;
			}
			else {
				set_Time(new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year);

				lcd_clear();
				sprintf(new_date_details_msg, "%02d-%02d-20%02d", new_dom, new_month, new_year);
				lcd_send_string("Ustawiona data:");
				lcd_second_line();
				lcd_send_string(new_date_details_msg);
				HAL_Delay(1000);
				lcd_clear();

				date_set_flag = 0;
			}

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

			if( (days_to_alarm < 0 || hours_to_alarm < 0 || minutes_to_alarm < 0 || seconds_to_alarm < 0) && alarm_set_flag) {
				lcd_clear();
				lcd_set_cursor(0, 4);
				lcd_send_string("B");
				lcd_send_own_char(3);
				lcd_send_own_char(2);
				lcd_send_string("dny czas");
				lcd_second_line();
				lcd_send_string("alarmu!");
				HAL_Delay(1000);
				lcd_clear();

				alarm_set_flag = 0;
			}
			else {
				rtc_set_alarm(days_to_alarm, hours_to_alarm, minutes_to_alarm, seconds_to_alarm);

				lcd_clear();
				sprintf(alarm_details_msg, "%02d:%02d:%02d", temp_hours, temp_mins, temp_sec);
				lcd_send_string("Alarm na godz.:");
				lcd_second_line();
				lcd_send_string(alarm_details_msg);
				HAL_Delay(1000);
				lcd_clear();

				alarm_set_flag = 0;
			}

		}

		HAL_Delay(1000);
	}

}

void go_to_standby(void) {

	// wykonaj jeśli zaczął się alarm
	if(alarm_flag) {

		while (alarm_flag) {
			to_do_on_alarm();
		}

		to_do_on_alarm_off();
	}

	// Przed wejściem w tryb STANDBY, musimy wyczyścić flagę WU (Wake up)
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	// Jeśli urzywamy RTC do wybudzenia MCU, w tym projekcie wybudza ALARM A, musimy wyczyścić też flagę RTC Wake up
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

	lcd_clear();
	lcd_set_cursor(0, 2);
	lcd_send_string("Entering to");
	lcd_second_line();
	lcd_send_string("STANDBY MODE...");
	HAL_Delay(1000);
	lcd_clear();

	// Przed wejściem w tryb STANDBY, musimy włączyć pin Wake up, aby mógł nas potem wybudzić z tego stanu
	HAL_PWR_EnableWakeUpPin(WakeUp_PIN_Pin);


	// Wybudzanie układu z trybu STANDBY, defaultowo co 30 sekund

	// Włączenie RTC Wake up			  0x1D4C0 = 120_000 = 30 sekund,	4000 = 1s
	if(HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x1D4C0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK) {
		Error_Handler();
	}

	lcd_clear();
	lcd_set_cursor(0, 2);
	lcd_send_string("STANDBY MODE");
	lcd_second_line();
	lcd_set_cursor(1, 7);
	lcd_send_string("ON");
	HAL_Delay(1000);
	lcd_back_light_off();
	lcd_clear();

	// Finalnie wchodzimy w tryb STANDBY
	HAL_PWR_EnterSTANDBYMode();
}
