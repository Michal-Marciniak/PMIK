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

/**
 * @brief Funkcja konwertująca wartość dziesiętną na binarną,
 * ponieważ dane zapisywane do rejestrów DS3231 muszą być postaci binarnej
 * @param val Wartość dziesiętna
 */
uint8_t decToBcd(int val);

/**
 * @brief Funkcja konwertująca wartość binarną na dziesiętną,
 * ponieważ dane odczytywane z DS3231 są postaci binarnej, a dane wyświetlane na LCD będą postaci dziesiętnej
 * @param val Wartość binarna
 */
int bcdToDec(uint8_t val);

/**
 * @brief Funkcja ustawiająca nową datę i godzinę zegara RTC
 * @param sec Sekunda
 * @param min Minuta
 * @param hour Godzina
 * @param dow Numer dnia tygodnia (1-pon. 2-wt. ...)
 * @param dom Numer dnia miesiąca (1, 30, 22 ...)
 * @param month Numer miesiąca (1-styczeń, 2-luty, ...)
 * @param year Rok (18, 19, 20 ...)
 */
void set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);

/**
 * @brief Funkcja sczytująca datę i godzinę z modułu DS3231
 */
void get_Time (void);

/**
 * @brief Funkcja zatrzymująca działanie programu, na określony przez użytkownika czas.
 * W porównaniu do systemowej funkcji HAL_Delay(), funkcja ta obsługuje system przerwań
 * @param delay Liczba milisekund
 */
void delay(uint16_t delay);

#endif /* INC_TIME_H_ */
