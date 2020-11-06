#include "lcd_i2c.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include "i2c.h"
#include "lcd_own_chars.h"
#include "time.h"

// RS = 0, wysyłamy instrukcje do LCD
// RS = 1, wysyłamy dane do LCD

uint8_t BL;		// Back light
uint8_t uart_flag;

uint8_t battery_level;

// tablica przechowująca czas i datę, wyświetlane na LCD
char time_date_buffer[12];

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

void lcd_init()
{
	/* sekwencja z dokumentacji inicjalizująca wyświetlacz*/
	HAL_Delay(40);
	lcd_write_command(INIT_8_BIT_MODE);
	HAL_Delay(5);
	lcd_write_command(INIT_8_BIT_MODE);
	HAL_Delay(1);
	lcd_write_command(INIT_8_BIT_MODE);

	/* ustawienie trybu 4-bitowego */
	lcd_write_command(INIT_4_BIT_MODE);

	/* włączenie kursora, bez migania i podkreślenia */
	lcd_write_command(UNDERLINE_OFF_BLINK_OFF);

	HAL_Delay(5);

	lcd_generate_own_chars();

	/* wyczyszczenie wyświetlacza */
	lcd_clear();
}

// Funkcja odpowiedzialna za aktualizowanie i wyświetlanie na bieżąco wszystkich danych na wyświetlaczu LCD
void lcd_display_refresh() {

	get_Time();

	// wyświetlanie godziny na LCD
	lcd_first_line();
	sprintf(time_date_buffer, "%02d:%02d:%02d", time.hour, time.minutes, time.seconds);
	lcd_send_string(time_date_buffer);

	//wyświetlenie poziomu baterii
	battery_level = 100;
	lcd_show_battery_level(battery_level);

	// wyświetlanie daty na LCD
	lcd_second_line();
	sprintf(time_date_buffer, "%02d-%02d-20%02d", time.dayofmonth, time.month, time.year);
	lcd_send_string(time_date_buffer);

	// wyświetlanie nazwy dnia tygodnia na LCD
	lcd_show_week_day_name(time.dayofweek);
}

void lcd_clear(void) {
	lcd_write_command(CLEAR_LCD);
	HAL_Delay(5);
}

// Funkcja odpowiadająca za włączenie podświetlenia wyświetlacza LCD
void lcd_back_light_on(void) {
	BL = 0x08;
}

// Funkcja odpowiadająca za wyłączenie podświetlenia wyświetlacza LCD
void lcd_back_light_off(void) {
	BL = 0x00;
}

// ustawienie kursora w danym wierszu i kolumnie
void lcd_set_cursor(uint8_t row, uint8_t col) {

	uint8_t col_number;
	col_number = (col) & 0x0F;

	switch(row) {

		case 0:
			col_number |= (0x80);
			lcd_write_command(col_number);	// ustawienie kursora w kolumnie x w pierwszym wierszu
			break;

		case 1:
			col_number |= (0xc0);
			lcd_write_command(col_number);	// ustawienie kursora w kolumnie x w drugim wierszu
			break;
	}

	HAL_Delay(5);
}

// ustawienie kursora na początku pierwszej linni
void lcd_first_line(void) {
	lcd_write_command(FIRST_LINE);
	HAL_Delay(5);
}

// ustawienie kursora na początku drugiej linni
void lcd_second_line(void) {
	lcd_write_command(SECOND_LINE);
	HAL_Delay(5);
}

// wysłanie instrukcji do wyświetlacza
void lcd_write_command(uint8_t data)
{
	uint8_t addr = LCD_ADDRESS;
	uint8_t tx_data[4];

	tx_data[0] = (data & 0xF0) | EN_PIN | BL;		// EN = 1, RS = 0
	tx_data[1] = (data & 0xF0) | BL;		// EN = 0, RS = 0  zatrzaśnięcie danych w wyświetlaczu
	tx_data[2] = (data << 4)   | EN_PIN | BL;		// EN = 1, RS = 0
	tx_data[3] = (data << 4)   | BL;		// EN = 0, RS = 0  zatrzaśnięcie danych w wyświetlaczu

	/* send data via i2c */
	HAL_I2C_Master_Transmit(&I2C_HANDLER, addr, tx_data, 4, 100);

	HAL_Delay(5);
}

// wysłanie danych do wyświetlacza
void lcd_write_data(uint8_t data)
{
	uint8_t addr = LCD_ADDRESS;
	uint8_t tx_data[4];
	uint8_t RS = 0x01;

	tx_data[0] = (data & 0xF0) | EN_PIN | RS | BL;		// EN = 1, RS = 1
	tx_data[1] = (data & 0xF0) | RS | BL;		// EN = 0, RS = 1  zatrzaśnięcie danych w wyświetlaczu
	tx_data[2] = (data << 4)   | EN_PIN | RS | BL;		// EN = 1, RS = 1
	tx_data[3] = (data << 4)   | RS | BL;		// EN = 0, RS = 1  zatrzaśnięcie danych w wyświetlaczu

	/* send data via i2c */
	HAL_I2C_Master_Transmit(&I2C_HANDLER, addr, tx_data, 4, 100);

	HAL_Delay(5);
}

void lcd_send_string(const char * string)
{
	while(*string)
	{
		lcd_write_data(*(string++));
	}

	HAL_Delay(5);
}

void lcd_send_alarm_on_msg(void) {

	lcd_clear();
	lcd_send_string("Wy");
	lcd_send_own_char(3);
	lcd_send_own_char(8);
	lcd_send_string("cz alarm !!!");
	HAL_Delay(1000);
}

void lcd_send_alarm_off_msg(void) {

	lcd_clear();
	lcd_send_string("Alarm wy");
	lcd_send_own_char(3);
	lcd_send_own_char(8);
	lcd_send_string("czony");
	HAL_Delay(2000);
	lcd_clear();
}

// funkcja do zapisania w pamięci CG RAM, ośmiu zdefiniowanych przez nas znaków
void lcd_generate_own_chars(void) {

	uint8_t i, j;

	lcd_write_command(CG_RAM_ADDRESS);	// Przechodzimy na początek pamięci CG RAM

	for(i = 0; i <= 7; i ++) {
		for(j = 0; j <= 7; j ++) {
			lcd_write_data(own_chars[i][j]);	// Zapisujemy własne znaki do pamięci CG RAM
		}
	}
}

// funkcja wyświetlająca wybrany znak własny
void lcd_send_own_char(uint8_t char_number) {

	switch(char_number) {

		case 1:
			lcd_send_string("\x1");
			break;

		case 2:
			lcd_send_string("\x2");
			break;

		case 3:
			lcd_send_string("\x3");
			break;

		case 4:
			lcd_send_string("\x4");
			break;

		case 5:
			lcd_send_string("\x5");
			break;

		case 6:
			lcd_send_string("\x6");
			break;

		case 7:
			lcd_send_string("\x7");
			break;

		case 8:
			lcd_send_string("\x8");
			break;
	}
}

// Funkcja odpowiedzialna za wyświetlenie nazwy dnia tygodnia
void lcd_show_week_day_name(uint8_t day_number) {

	lcd_set_cursor(1, 12);

	switch(day_number) {

		case 1:
			lcd_send_string("pon.");
			break;

		case 2:
			lcd_send_string("wt.");
			break;

		case 3:
			//lcd_send_own_char(6);
			lcd_send_string("sr.");
			break;

		case 4:
			lcd_send_string("czw.");
			break;

		case 5:
			lcd_send_string("pt.");
			break;

		case 6:
			lcd_send_string("sob.");
			break;

		case 7:
			lcd_send_string("ndz.");
			break;
	}
}

void lcd_show_battery_level(uint8_t bat_lev) {

	lcd_set_cursor(0, 11);
	lcd_send_own_char(6);

	char battery_level[4];
	sprintf(battery_level, "%d", bat_lev);

	lcd_send_string(battery_level);
	lcd_send_string("%");
}


