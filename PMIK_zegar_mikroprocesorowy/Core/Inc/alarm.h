/*
 * alarm.h
 *
 *  Created on: Oct 25, 2020
 *      Author: michal
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_

/**
 * @brief Funkcja sczytująca dane z modułu RTC DS3231, oraz ustawiająca odpowiednią datę i godzinę zegara RTC.
 */
void rtc_set_time (void);

/**
 * @brief Funkcja ustawiająca alarm, na zadaną przez użytkownika godzinę.
 * @param hour Godzina alarmu
 * @param min Minuta alarmu
 * @param sec Sekunda alarmu
 * @param day Dzień tygodnia alarmu (pon. wt. ...)
 */
void rtc_set_alarm (uint8_t hour, uint8_t min, uint8_t sec, uint8_t day);

/**
 * @brief Funkcja ustawiająca flagę zmiany godziny
 */
void activate_time (void);

/**
 * @brief Funkcja ustawiająca flagę zmiany daty
 */
void activate_date (void);

/**
 * @brief Funkcja ustawiająca flagę zmiany alarmu
 */
void activate_alarm (void);

/**
 * @brief Funkcja ustawiająca flagę włączenia alarmu
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

/**
 * @brief Funkcja odpowiedzialna za obsługę programu, podczas alarmu
 */
void to_do_on_alarm(void);

/**
 * @brief Funkcja odpowiedzialna za obsługę programu, po wyłączeniu alarmu
 */
void to_do_on_alarm_off(void);

/**
 * @brief Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
 * @param GPIO_Pin Pin wywołujący przerwanie
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* INC_ALARM_H_ */
