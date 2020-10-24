#include "lcd_i2c.h"
#include "stm32f4xx_hal.h"

#include "i2c.h"
#include "lcd_own_chars.h"

// RS = 0, wysyłamy instrukcje do LCD
// RS = 1, wysyłamy dane do LCD

void lcd_init()
{
	/* sekwencja inicjalizująca wyświetlacz, z dokumentacji */
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

	lcd_generate_own_chars();

	/* wyczyszczenie wyświetlacza */
	lcd_clear();
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

	tx_data[0] = (data & 0xF0) | EN_PIN | BL_PIN;		// EN = 1, RS = 0
	tx_data[1] = (data & 0xF0) | BL_PIN;		// EN = 0, RS = 0  zatrzaśnięcie danych w wyświetlaczu
	tx_data[2] = (data << 4)   | EN_PIN | BL_PIN;		// EN = 1, RS = 0
	tx_data[3] = (data << 4)   | BL_PIN;		// EN = 0, RS = 0  zatrzaśnięcie danych w wyświetlaczu

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

	tx_data[0] = (data & 0xF0) | EN_PIN | RS | BL_PIN;		// EN = 1, RS = 1
	tx_data[1] = (data & 0xF0) | RS | BL_PIN;		// EN = 0, RS = 1  zatrzaśnięcie danych w wyświetlaczu
	tx_data[2] = (data << 4)   | EN_PIN | RS | BL_PIN;		// EN = 1, RS = 1
	tx_data[3] = (data << 4)   | RS | BL_PIN;		// EN = 0, RS = 1  zatrzaśnięcie danych w wyświetlaczu

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

	switch(day_number) {

		case 1:
			lcd_send_string("pon.");
			break;

		case 2:
			lcd_send_string("wt.");
			break;

		case 3:
			lcd_send_own_char(7);
			lcd_send_string("r.");
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
			lcd_send_string("niedz.");
			break;

	}
}

void lcd_clear(void)
{
	/* clear display */
	lcd_write_command(CLEAR_LCD);
	HAL_Delay(5);
}
