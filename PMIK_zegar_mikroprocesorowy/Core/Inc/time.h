/*
 * time.h
 *
 *  Created on: Oct 25, 2020
 *      Author: michal
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include <stdio.h>
#include <stdint.h>

// Funkcja konwertująca wartość dziesiętną na binarną,
// ponieważ dane zapisywane do rejestrów DS3231 muszą być postaci binarnej
uint8_t decToBcd(int val);

// Funkcja konwertująca wartość binarną na decymalną,
// ponieważ dane odczytywane z DS3231 są postaci binarnej, a dane wyświetlane na LCD będą postaci dziesiętnej
int bcdToDec(uint8_t val);

// Funkcje do ustawiania i pobierania czasu i daty
// Przekazywane wartości muszą być typu binarnego
void set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);

void get_Time (void);

void delay(uint16_t delay);

#endif /* INC_TIME_H_ */
