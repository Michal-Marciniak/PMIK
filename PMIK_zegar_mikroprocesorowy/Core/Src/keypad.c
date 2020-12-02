/*
 * keypad.c
 *
 *  Created on: Nov 27, 2020
 *      Author: michal
 */

#include "usart.h"
#include "keypad.h"

static Keypad_Wires_TypeDef KeypadStruct;

// tablica, przechowująca numery pinów danych kolumn
static uint8_t ColumnsPositions[4];

uint8_t idx;

static char *keypad_keys[16] =
{
	"1",
	"2",
	"3",
	"A",
	"4",
	"5",
	"6",
	"B",
	"7",
	"8",
	"9",
	"C",
	"*",
	"0",
	"#",
	"D"
};

void keypad4x4_Init(Keypad_Wires_TypeDef  *KeypadWiringStruct)
{
	KeypadStruct = *KeypadWiringStruct;

	// znalezienie pinów 4 kolumn
	keypad4x4_FindPins_positions();

	// ustawienie pinów wszystkich kolumn na stan wysoki
	HAL_GPIO_WritePin(KeypadStruct.C1_Port, KeypadStruct.C1_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KeypadStruct.C2_Port, KeypadStruct.C2_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KeypadStruct.C3_Port, KeypadStruct.C3_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(KeypadStruct.C4_Port, KeypadStruct.C4_pin, GPIO_PIN_SET);
}

void keypad4x4_FindPins_positions(void)
{
	idx=0;

	for(idx=0; idx<16; idx++)
	{
		if(((KeypadStruct.C1_pin >> idx) & 0x0001) == 0x0001)
		{
			ColumnsPositions[0] = idx;
		}
		if(((KeypadStruct.C2_pin >> idx) & 0x0001) == 0x0001)
		{
			ColumnsPositions[1] = idx;
		}
		if(((KeypadStruct.C3_pin >> idx) & 0x0001) == 0x0001)
		{
			ColumnsPositions[2] = idx;
		}
		if(((KeypadStruct.C4_pin >> idx) & 0x0001) == 0x0001)
		{
			ColumnsPositions[3] = idx;
		}
	}
}

void keypad4x4_ChangeColumn(uint8_t col_num)
{
	if(col_num == 1)
	{
		//Set selected column
		KeypadStruct.C1_Port->OTYPER &= ~(1UL << ColumnsPositions[0]);

		//Make other columns floating
		KeypadStruct.C2_Port->OTYPER |= (1UL << ColumnsPositions[1]);
		KeypadStruct.C3_Port->OTYPER |= (1UL << ColumnsPositions[2]);
		KeypadStruct.C4_Port->OTYPER |= (1UL << ColumnsPositions[3]);
	}
	else if(col_num == 2)
	{
		//Set selected column
		KeypadStruct.C2_Port->OTYPER &= ~(1UL << ColumnsPositions[1]);

		//Make other columns floating
		KeypadStruct.C1_Port->OTYPER |= (1UL << ColumnsPositions[0]);
		KeypadStruct.C3_Port->OTYPER |= (1UL << ColumnsPositions[2]);
		KeypadStruct.C4_Port->OTYPER |= (1UL << ColumnsPositions[3]);
	}
	else if(col_num == 3)
	{
		//Set selected column
		KeypadStruct.C3_Port->OTYPER &= ~(1UL << ColumnsPositions[2]);

		//Make other columns floating
		KeypadStruct.C1_Port->OTYPER |= (1UL << ColumnsPositions[0]);
		KeypadStruct.C2_Port->OTYPER |= (1UL << ColumnsPositions[1]);
		KeypadStruct.C4_Port->OTYPER |= (1UL << ColumnsPositions[3]);
	}
	else if(col_num == 4)
	{
		//Set selected column
		KeypadStruct.C4_Port->OTYPER &= ~(1UL << ColumnsPositions[3]);

		//Make other columns floating
		KeypadStruct.C1_Port->OTYPER |= (1UL << ColumnsPositions[0]);
		KeypadStruct.C2_Port->OTYPER |= (1UL << ColumnsPositions[1]);
		KeypadStruct.C3_Port->OTYPER |= (1UL << ColumnsPositions[2]);
	}
}

void keypad4x4_ReadKeypad(bool keys[16])
{
	// Ustawienie pierwszej kolumny na stan wysoki, a następnie sprawdzanie wszystkich wierszy
	keypad4x4_ChangeColumn(1);
	keys[0] = HAL_GPIO_ReadPin(KeypadStruct.R1_Port, KeypadStruct.R1_pin);
	keys[4] = HAL_GPIO_ReadPin(KeypadStruct.R2_Port, KeypadStruct.R2_pin);
	keys[8] = HAL_GPIO_ReadPin(KeypadStruct.R3_Port, KeypadStruct.R3_pin);
	keys[12] = HAL_GPIO_ReadPin(KeypadStruct.R4_Port, KeypadStruct.R4_pin);

	// Ustawienie drugiej kolumny na stan wysoki, a następnie sprawdzanie wszystkich wierszy
	keypad4x4_ChangeColumn(2);
	keys[1] = HAL_GPIO_ReadPin(KeypadStruct.R1_Port, KeypadStruct.R1_pin);
	keys[5] = HAL_GPIO_ReadPin(KeypadStruct.R2_Port, KeypadStruct.R2_pin);
	keys[9] = HAL_GPIO_ReadPin(KeypadStruct.R3_Port, KeypadStruct.R3_pin);
	keys[13] = HAL_GPIO_ReadPin(KeypadStruct.R4_Port, KeypadStruct.R4_pin);

	// Ustawienie trzeciej kolumny na stan wysoki, a następnie sprawdzanie wszystkich wierszy
	keypad4x4_ChangeColumn(3);
	keys[2] = HAL_GPIO_ReadPin(KeypadStruct.R1_Port, KeypadStruct.R1_pin);
	keys[6] = HAL_GPIO_ReadPin(KeypadStruct.R2_Port, KeypadStruct.R2_pin);
	keys[10] = HAL_GPIO_ReadPin(KeypadStruct.R3_Port, KeypadStruct.R3_pin);
	keys[14] = HAL_GPIO_ReadPin(KeypadStruct.R4_Port, KeypadStruct.R4_pin);

	// Ustawienie czwartej kolumny na stan wysoki, a następnie sprawdzanie wszystkich wierszy
	keypad4x4_ChangeColumn(4);
	keys[3] = HAL_GPIO_ReadPin(KeypadStruct.R1_Port, KeypadStruct.R1_pin);
	keys[7] = HAL_GPIO_ReadPin(KeypadStruct.R2_Port, KeypadStruct.R2_pin);
	keys[11] = HAL_GPIO_ReadPin(KeypadStruct.R3_Port, KeypadStruct.R3_pin);
	keys[15] = HAL_GPIO_ReadPin(KeypadStruct.R4_Port, KeypadStruct.R4_pin);
}

char* keypad4x4_GetChar(uint8_t keypad_char_index)
{
	return keypad_keys[keypad_char_index];
}


