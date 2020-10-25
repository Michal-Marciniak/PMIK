/*
 * time.c
 *
 *  Created on: Oct 25, 2020
 *      Author: michal
 */

#include "stm32f4xx_hal.h"
#include "i2c.h"

#define DS3231_ADDRESS 0xD0

// Funkcja konwertująca wartość dziesiętną na binarną,
// ponieważ dane zapisywane do rejestrów DS3231 muszą być postaci binarnej
uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}

// Funkcja konwertująca wartość binarną na decymalną,
// ponieważ dane odczytywane z DS3231 są postaci binarnej, a dane wyświetlane na LCD będą postaci dziesiętnej
int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

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

// Funkcje do ustawiania i pobierania czasu i daty
// Przekazywane wartości muszą być typu binarnego
void set_Time (uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t time_to_write[7];

	time_to_write[0] = decToBcd(sec);
	time_to_write[1] = decToBcd(min);
	time_to_write[2] = decToBcd(hour);
	time_to_write[3] = decToBcd(dow);
	time_to_write[4] = decToBcd(dom);
	time_to_write[5] = decToBcd(month);
	time_to_write[6] = decToBcd(year);

	HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, time_to_write, 7, 1000);
}

void get_Time (void)
{
	uint8_t time_to_red[7];

	HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, time_to_red, 7, 1000);

	time.seconds = bcdToDec(time_to_red[0]);
	time.minutes = bcdToDec(time_to_red[1]);
	time.hour = bcdToDec(time_to_red[2]);
	time.dayofweek = bcdToDec(time_to_red[3]);
	time.dayofmonth = bcdToDec(time_to_red[4]);
	time.month = bcdToDec(time_to_red[5]);
	time.year = bcdToDec(time_to_red[6]);
}

