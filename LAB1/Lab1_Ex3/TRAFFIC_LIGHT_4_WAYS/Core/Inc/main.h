/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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
#define N_RED_Pin GPIO_PIN_0
#define N_RED_GPIO_Port GPIOA
#define N_YELLOW_Pin GPIO_PIN_1
#define N_YELLOW_GPIO_Port GPIOA
#define N_GREEN_Pin GPIO_PIN_2
#define N_GREEN_GPIO_Port GPIOA
#define S_RED_Pin GPIO_PIN_3
#define S_RED_GPIO_Port GPIOA
#define S_YELLOW_Pin GPIO_PIN_4
#define S_YELLOW_GPIO_Port GPIOA
#define S_GREEN_Pin GPIO_PIN_5
#define S_GREEN_GPIO_Port GPIOA
#define W_RED_Pin GPIO_PIN_0
#define W_RED_GPIO_Port GPIOB
#define W_YELLOW_Pin GPIO_PIN_1
#define W_YELLOW_GPIO_Port GPIOB
#define W_GREEN_Pin GPIO_PIN_2
#define W_GREEN_GPIO_Port GPIOB
#define E_RED_Pin GPIO_PIN_8
#define E_RED_GPIO_Port GPIOA
#define E_YELLOW_Pin GPIO_PIN_9
#define E_YELLOW_GPIO_Port GPIOA
#define E_GREEN_Pin GPIO_PIN_10
#define E_GREEN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
