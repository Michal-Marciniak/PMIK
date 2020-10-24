#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include <stdbool.h>
#include <stdint.h>

/*
 *         PCF8574 <-> HD44780
 *
 * I2C I/O   P7 P6 P5 P4 P3 P2 P1 P0
 * LCD       D7 D6 D5 D4 BL EN RW RS
 *
 * */

#define LCD_ADDRESS (0x27 << 1)
#define I2C_HANDLER hi2c1

#define RS_PIN 0x01
#define RW_PIN 0x02
#define EN_PIN 0x04
#define BL_PIN 0x08

#define INIT_8_BIT_MODE	0x30
#define INIT_4_BIT_MODE	0x02

#define CLEAR_LCD	0x01

#define UNDERLINE_OFF_BLINK_OFF		0x0C
#define UNDERLINE_OFF_BLINK_ON		0x0D
#define UNDERLINE_ON_BLINK_OFF		0x0E
#define UNDERLINE_ON_BLINK_ON		0x0F

#define FIRST_LINE	0x80
#define SECOND_LINE	0xC0

struct lcd_disp {
	char first_line[17];
	char second_line[17];
};

void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_first_line(void);
void lcd_second_line(void);
void lcd_write_command(uint8_t data);
void lcd_write_data(uint8_t data);
void lcd_send_string(const char * string);
void lcd_generate_own_chars(void);
void lcd_send_own_char(uint8_t char_num);
void lcd_show_week_day_name(uint8_t day_number);
void lcd_clear(void);

#endif /* INC_LCD_I2C_H_ */
