/*
 * keypad.h
 *
 *  Created on: Nov 27, 2020
 *      Author: michal
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

/**
 * @brief Struktura przechowująca nazwy pinów oraz portów, przycisków klawiatury
 */
typedef struct
{
	// Ports
	GPIO_TypeDef* R1_Port;
	GPIO_TypeDef* R2_Port;
	GPIO_TypeDef* R3_Port;
	GPIO_TypeDef* R4_Port;

	GPIO_TypeDef* C1_Port;
	GPIO_TypeDef* C2_Port;
	GPIO_TypeDef* C3_Port;
	GPIO_TypeDef* C4_Port;

	// Pins
	uint16_t	R1_pin;
	uint16_t	R2_pin;
	uint16_t	R3_pin;
	uint16_t	R4_pin;

	uint16_t	C1_pin;
	uint16_t	C2_pin;
	uint16_t	C3_pin;
	uint16_t	C4_pin;
} Keypad_Wires_TypeDef;

/**
 * @brief Funkcja inicjalizująca klawiaturę 4x4
 * @param KeypadWiringStruct Wskaźnik na strukturę przechowującą piny oraz porty klawiszy
 */
void keypad4x4_Init(Keypad_Wires_TypeDef  *KeypadWiringStruct);

/**
 * @brief Funkcja wykrywająca kolumnę, która zawiera wciśnięty klawisz
 */
void keypad4x4_FindPins_positions(void);

/**
 * @brief Funkcja zmieniająca numer odczytywanej kolumny
 */
void keypad4x4_ChangeColumn(uint8_t colNum_0_to_3);

/**
 * @brief Funkcja odpowiedzialna za odczytanie aktualnie wciśniętego przycisku
 */
void keypad4x4_ReadKeypad(bool keys[16]);

/**
 * @brief Funkcja odpowiedzialna za zwrócenie aktualnie wciśniętego znaku
 */
char* keypad4x4_GetChar(uint8_t keypad_char_index);

#endif /* INC_KEYPAD_H_ */
