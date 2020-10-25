/*
 * alarm.h
 *
 *  Created on: Oct 25, 2020
 *      Author: michal
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_

void rtc_set_time (void);

void rtc_set_alarm (uint8_t hour, uint8_t min, uint8_t sec, uint8_t day);

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

void to_do_on_alarm(void);

// Funkcja odpowiedzialna za wyłączenie alarmu, za pomocą niebieskiego przycisku
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif /* INC_ALARM_H_ */
