/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
  typedef enum { COLOR_RED, COLOR_YELLOW, COLOR_GREEN } Color;
  typedef struct{
	  Color ns;
	  Color ew;
	  uint32_t duration;
  } Phase;

  typedef struct {
      GPIO_TypeDef* port;
      uint16_t pin;
  } PinRef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  Phase phaseTable[4] = {
		  { COLOR_GREEN, 	COLOR_RED, 		5000},
		  { COLOR_YELLOW,	COLOR_RED, 		2000},
		  { COLOR_RED,		COLOR_GREEN,	5000},
		  { COLOR_RED,		COLOR_YELLOW,	2000}
  };

  int currentPhase = 0;

  PinRef segPins[7] = {
      {SEG_A_GPIO_Port, SEG_A_Pin},
      {SEG_B_GPIO_Port, SEG_B_Pin},
      {SEG_C_GPIO_Port, SEG_C_Pin},
      {SEG_D_GPIO_Port, SEG_D_Pin},
      {SEG_E_GPIO_Port, SEG_E_Pin},
      {SEG_F_GPIO_Port, SEG_F_Pin},
      {SEG_G_GPIO_Port, SEG_G_Pin}
  };

  const uint8_t seg_table[10] = {
      0x40, 0x79, 0x24, 0x30, 0x19,
      0x12, 0x02, 0x78, 0x00, 0x10
  };

  uint32_t phaseStartTime = 0;
  int lastDisplayedSecond = -1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void setDirection(Color c, GPIO_TypeDef *port, uint16_t redPin, uint16_t yellowPin, uint16_t greenPin);
void enterPhase(int phaseIndex);

void display7SEG(int num);
void updateSystem(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
  phaseStartTime = HAL_GetTick();
  enterPhase(currentPhase);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */
	  updateSystem();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, N_RED_Pin|N_YELLOW_Pin|N_GREEN_Pin|S_RED_Pin
                          |S_YELLOW_Pin|S_GREEN_Pin|E_RED_Pin|E_YELLOW_Pin
                          |E_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, W_RED_Pin|W_YELLOW_Pin|W_GREEN_Pin|SEG_A_Pin
                          |SEG_B_Pin|SEG_C_Pin|SEG_D_Pin|SEG_E_Pin
                          |SEG_F_Pin|SEG_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : N_RED_Pin N_YELLOW_Pin N_GREEN_Pin S_RED_Pin
                           S_YELLOW_Pin S_GREEN_Pin E_RED_Pin E_YELLOW_Pin
                           E_GREEN_Pin */
  GPIO_InitStruct.Pin = N_RED_Pin|N_YELLOW_Pin|N_GREEN_Pin|S_RED_Pin
                          |S_YELLOW_Pin|S_GREEN_Pin|E_RED_Pin|E_YELLOW_Pin
                          |E_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : W_RED_Pin W_YELLOW_Pin W_GREEN_Pin SEG_A_Pin
                           SEG_B_Pin SEG_C_Pin SEG_D_Pin SEG_E_Pin
                           SEG_F_Pin SEG_G_Pin */
  GPIO_InitStruct.Pin = W_RED_Pin|W_YELLOW_Pin|W_GREEN_Pin|SEG_A_Pin
                          |SEG_B_Pin|SEG_C_Pin|SEG_D_Pin|SEG_E_Pin
                          |SEG_F_Pin|SEG_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void setDirection(Color c, GPIO_TypeDef *port,
                   uint16_t redPin, uint16_t yellowPin, uint16_t greenPin) {
    HAL_GPIO_WritePin(port, redPin,    (c == COLOR_RED)    ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(port, yellowPin, (c == COLOR_YELLOW) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(port, greenPin,  (c == COLOR_GREEN)  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void enterPhase(int phaseIndex) {
    Phase p = phaseTable[phaseIndex];
    setDirection(p.ns, GPIOA, N_RED_Pin, N_YELLOW_Pin, N_GREEN_Pin);
    setDirection(p.ns, GPIOA, S_RED_Pin, S_YELLOW_Pin, S_GREEN_Pin);
    setDirection(p.ew, GPIOA, E_RED_Pin, E_YELLOW_Pin, E_GREEN_Pin);
    setDirection(p.ew, GPIOB, W_RED_Pin, W_YELLOW_Pin, W_GREEN_Pin);
}

// ==== Exercise 4 ====
void display7SEG(int num) {
    if (num < 0 || num > 9) return;
    uint8_t pattern = seg_table[num];
    for (int i = 0; i < 7; i++) {
        GPIO_PinState state = ((pattern >> i) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(segPins[i].port, segPins[i].pin, state);
    }
}

void updateSystem(void) {
    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - phaseStartTime;
    uint32_t duration = phaseTable[currentPhase].duration;

    if (elapsed >= duration) {
        currentPhase = (currentPhase + 1) % 4;
        phaseStartTime = now;
        enterPhase(currentPhase);
        elapsed = 0;
        duration = phaseTable[currentPhase].duration;
    }


    int secondsLeft = (duration - elapsed) / 1000 + 1;
    if (secondsLeft > 9) secondsLeft = 9;

    if (secondsLeft != lastDisplayedSecond) {
        display7SEG(secondsLeft);
        lastDisplayedSecond = secondsLeft;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
