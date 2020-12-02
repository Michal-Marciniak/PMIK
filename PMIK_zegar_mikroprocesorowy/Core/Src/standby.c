/*
 * standby_init.c
 *
 *  Created on: Nov 4, 2020
 *      Author: michal
 */

#include "main.h"
#include "tim.h"
#include "rtc.h"
#include "usart.h"

#include "lcd_i2c.h"
#include "time.h"
#include "alarm.h"
#include "keypad.h"


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
uint8_t new_sec, new_min, new_hour, new_dow, new_dom, new_month, new_year;
char new_time_details_msg[12];
/*	time	*/

/*	date	*/
uint8_t date_activated_flag;	// flaga sygnalizująca, że w tym momencie użytkownik ustawia nową date
uint8_t date_set_flag;			// flaga sygnalizująca, że nowa data została pomyślnie ustawiona
char new_date_details_msg[12];
/*	date	*/

/*	alarm	*/
uint8_t alarm_activated_flag;	// flaga sygnalizująca, że w tym momencie użytkownik ustawia nowy alarm
uint8_t alarm_set_flag; 		// flaga sygnalizująca, że alarm został pomyślnie ustawiony
uint8_t alarm_flag; 			// flaga sygnalizująca włączenie się alarmu

uint8_t days_to_alarm, alarm_hour, alarm_min, alarm_sec;
/*	alarm	*/

/*	keypad	*/
bool keypadSwitches[16] = {0};
Keypad_Wires_TypeDef keypadStruct;
/*	keypad	*/


void to_do_after_wake_up_from_standby(void) {

	lcd_back_light_on();

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

	  for(int i=0; i<15; i++) {
		  HAL_GPIO_TogglePin(Green_LED_GPIO_Port, Green_LED_Pin);
		  delay(80);
	  }

	  if(!alarm_flag) {
		  lcd_clear();
		  lcd_set_cursor(0, 2);
		  lcd_send_string("Wake up from");
		  lcd_second_line();
		  lcd_set_cursor(1, 2);
		  lcd_send_string("STANDBY MODE");
		  delay(1000);
		  lcd_clear();
	  }
	  else {
		  to_do_on_alarm();
	  }

	  // Wyłączenie pinu WAKE UP
	  HAL_PWR_DisableWakeUpPin(WakeUp_PIN_Pin);

	  // Wyłączenie Wake up timer
	  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	}

	// Keypad ports
	keypadStruct.R1_Port = R1_GPIO_Port;
	keypadStruct.R2_Port = R2_GPIO_Port;
	keypadStruct.R3_Port = R3_GPIO_Port;
	keypadStruct.R4_Port = R4_GPIO_Port;

	keypadStruct.C1_Port = C1_GPIO_Port;
	keypadStruct.C2_Port = C2_GPIO_Port;
	keypadStruct.C3_Port = C3_GPIO_Port;
	keypadStruct.C4_Port = C4_GPIO_Port;

	// Keypad pins
	keypadStruct.R1_pin = R1_Pin;
	keypadStruct.R2_pin = R2_Pin;
	keypadStruct.R3_pin = R3_Pin;
	keypadStruct.R4_pin = R4_Pin;

	keypadStruct.C1_pin = C1_Pin;
	keypadStruct.C2_pin = C2_Pin;
	keypadStruct.C3_pin = C3_Pin;
	keypadStruct.C4_pin = C4_Pin;

	keypad4x4_Init(&keypadStruct);
}

void to_do_before_going_to_standby(void) {

	for(int i=0; i<3000; i++) {

		// wykonaj jeśli zaczął się alarm
		if(alarm_flag) {

			while (alarm_flag) {
				to_do_on_alarm();
			}

			to_do_on_alarm_off();
		}

		keypad4x4_ReadKeypad(keypadSwitches);

		for(int j=0; j<16; j++) {

			if(keypadSwitches[j]) {
				lcd_clear();
				HAL_Delay(5);
				lcd_send_string("Przycisk: ");
				lcd_send_string(keypad4x4_GetChar(j));
				delay(1000);
				lcd_clear();
			}

		}

		lcd_display_refresh();

		// wykonaj jeśli użytkownik ustawił nową godzinę
		if(time_set_flag) {

			new_sec = time.seconds;
			new_hour = global_buffer[1];
			new_min = global_buffer[2];
			new_dow = time.dayofweek;
			new_dom = time.dayofmonth;
			new_month = time.month;
			new_year = time.year;

			if( (new_hour > 23 || new_hour < 0 || new_min > 59 || new_min < 1) && time_set_flag) {

				lcd_clear();
				lcd_set_cursor(0, 1);
				lcd_send_string("Nieprawid");
				lcd_send_own_char(3);
				lcd_send_string("owy");
				lcd_second_line();
				lcd_set_cursor(1, 6);
				lcd_send_string("czas!");
				delay(1000);
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
				delay(1000);
				lcd_clear();

				time_set_flag = 0;
			}

		}

		// wykonaj jeśli użytkownik ustawił nową datę
		if(date_set_flag) {

			new_sec = time.seconds;
			new_min = time.minutes;
			new_hour = time.hour;

			new_dow = global_buffer[1];
			new_dom = global_buffer[2];
			new_month = global_buffer[3];
			new_year = global_buffer[4];


			if( (new_dow > 7 || new_dow < 1 || new_dom > 31 || new_dom < 1 || new_month > 12 || new_month < 1 || new_year < 20) && date_set_flag) {

				lcd_clear();
				lcd_set_cursor(0, 1);
				lcd_send_string("Nieprawid");
				lcd_send_own_char(3);
				lcd_send_string("owa");
				lcd_second_line();
				lcd_set_cursor(1, 6);
				lcd_send_string("data!");
				delay(1000);
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
				delay(1000);
				lcd_clear();

				date_set_flag = 0;
			}

		}

		// wykonaj jeśli użytkownik ustawił nowy czas alarmu
		if(alarm_set_flag) {

			days_to_alarm = global_buffer[1];	// dni to drugi element, ponieważ pierwszy to literka a, mówiąca o tym, że ustawiamy alarm.
			alarm_hour = global_buffer[2];
			alarm_min = global_buffer[3];
			alarm_sec = global_buffer[4];

			rtc_set_alarm(days_to_alarm, alarm_hour, alarm_min, alarm_sec);
			alarm_set_flag = 0;
		}

		delay(10);		// 10 ms
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
	delay(1000);
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
	delay(1000);
	lcd_back_light_off();
	lcd_clear();

	HAL_NVIC_SetPriority(EXTI_LINE_22, 0, 0);	// Wybudzenie z trybu STANDBY ma najwyższy priorytet w układzie

	// Finalnie wchodzimy w tryb STANDBY
	HAL_PWR_EnterSTANDBYMode();
}
