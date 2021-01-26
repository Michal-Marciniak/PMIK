#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include <stdint.h>

/**
 * PCF8574 <-> HD44780
 *
 * I2C I/O   P7 P6 P5 P4 P3 P2 P1 P0
 * LCD       D7 D6 D5 D4 BL EN RW RS
 *
 */

#define LCD_ADDRESS (0x27 << 1)
#define I2C_HANDLER hi2c1

#define RS_PIN 0x01
#define RW_PIN 0x02
#define EN_PIN 0x04
#define BL_PIN 0x00

#define INIT_8_BIT_MODE	0x30
#define INIT_4_BIT_MODE	0x02

#define CLEAR_LCD	0x01

#define UNDERLINE_OFF_BLINK_OFF		0x0C
#define UNDERLINE_OFF_BLINK_ON		0x0D
#define UNDERLINE_ON_BLINK_OFF		0x0E
#define UNDERLINE_ON_BLINK_ON		0x0F

#define FIRST_LINE	0x80
#define SECOND_LINE	0xC0

/**
 * @brief Funkcja inicjalizująca wyświetlacz LCD
 */
void lcd_init(void);

/**
 * @brief Funkcja odświeżająca dane wyświetlane na ekranie
 */
void lcd_display_refresh(void);

/**
 * @brief Funkcja czyszcząca ekran
 */
void lcd_clear(void);

/**
 * @brief Funkcja włączająca podświetlenie ekranu
 */
void lcd_back_light_on(void);

/**
 * @brief Funkcja wyłączająca podświetlenie ekranu
 */
void lcd_back_light_off(void);

/**
 * @brief Funkcja ustawiająca kursor ekranu na danej pozycji
 */
void lcd_set_cursor(uint8_t row, uint8_t col);

/**
 * @brief Funkcja ustawiająca kursor na początku pierwszego wiersza ekranu
 */
void lcd_first_line(void);

/**
 * @brief Funkcja ustawiająca kursor na początku drugiego wiersza ekranu
 */
void lcd_second_line(void);

/**
 * @brief Funkcja odpowiedzialna za wysyłanie instrukcji do wyświetlacza
 */
void lcd_write_command(uint8_t data);

/**
 * @brief Funkcja odpowiedzialna za wysyłanie danych do wyświetlacza
 */
void lcd_write_data(uint8_t data);

/**
 * @brief Funkcja wyświetlająca na ekranie napis
 * @param string Napis wyświetlany na ekranie
 */
void lcd_send_string(const char * string);

/**
 * @brief Funkcja wyświetlająca na ekranie informację o alarmie
 */
void lcd_send_alarm_on_msg(void);

/**
 * @brief Funkcja wyświetlająca na ekranie informację o wyłączeniu alarmu
 */
void lcd_send_alarm_off_msg(void);

/**
 * @brief Funkcja generująca do 8 własnych, zdefiniowanych przez użytkownika znaków
 */
void lcd_generate_own_chars(void);

/**
 * @brief Funkcja wyświetlająca na ekranie własny znak użytkownika
 * @param char_num Numer odpowiadający własnemu znakowi użytkownika
 */
void lcd_send_own_char(uint8_t char_num);

/**
 * @brief Funkcja wyświetlająca na ekranie nazwę aktualnego dnia tygodnia
 * @param day_number Numer dnia tygodnia (1 - pon.  2 - wt. ...)
 */
void lcd_show_week_day_name(uint8_t day_number);

/**
 * @brief Funkcja wyświetlająca na ekranie poziom naładowania baterii wyświetlacza
 * @param battery_level Poziom baterii wyrażony w procentach (np. 80)
 */
void lcd_show_battery_level(uint8_t battery_level);

#endif /* INC_LCD_I2C_H_ */
