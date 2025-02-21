/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Blue_Button_Pin GPIO_PIN_13
#define Blue_Button_GPIO_Port GPIOC
#define Blue_Button_EXTI_IRQn EXTI15_10_IRQn
#define WakeUp_PIN_Pin GPIO_PIN_0
#define WakeUp_PIN_GPIO_Port GPIOA
#define Green_LED_Pin GPIO_PIN_5
#define Green_LED_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_6
#define Buzzer_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_13
#define R1_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_14
#define R2_GPIO_Port GPIOB
#define BT_TX_Pin GPIO_PIN_6
#define BT_TX_GPIO_Port GPIOC
#define C4_Pin GPIO_PIN_7
#define C4_GPIO_Port GPIOC
#define C2_Pin GPIO_PIN_8
#define C2_GPIO_Port GPIOA
#define C3_Pin GPIO_PIN_9
#define C3_GPIO_Port GPIOA
#define R3_Pin GPIO_PIN_10
#define R3_GPIO_Port GPIOA
#define BT_RX_Pin GPIO_PIN_12
#define BT_RX_GPIO_Port GPIOA
#define C1_Pin GPIO_PIN_4
#define C1_GPIO_Port GPIOB
#define R4_Pin GPIO_PIN_5
#define R4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
