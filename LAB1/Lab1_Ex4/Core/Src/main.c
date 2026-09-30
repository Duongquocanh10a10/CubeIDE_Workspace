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
typedef enum {
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN
} Color;

typedef struct {
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
    { COLOR_GREEN,  COLOR_RED,     3000 },  // NS Green,  EW Red
    { COLOR_YELLOW, COLOR_RED,     2000 },  // NS Yellow, EW Red
    { COLOR_RED,    COLOR_GREEN,   3000 },  // NS Red,    EW Green
    { COLOR_RED,    COLOR_YELLOW,  2000 }   // NS Red,    EW Yellow
};

int currentPhase = 0;

uint32_t phaseStartTime = 0;

/* 7-segment table
 *
 * bit 0 -> segment A
 * bit 1 -> segment B
 * bit 2 -> segment C
 * bit 3 -> segment D
 * bit 4 -> segment E
 * bit 5 -> segment F
 * bit 6 -> segment G
 *
 * Common Anode
 */
const uint8_t seg_table[10] = {
    0x40,   // 0
    0x79,   // 1
    0x24,   // 2
    0x30,   // 3
    0x19,   // 4
    0x12,   // 5
    0x02,   // 6
    0x78,   // 7
    0x00,   // 8
    0x10    // 9
};

/* Bắc - Nam */
PinRef nsSegPins[7] = {
    {NS_SEG_A_GPIO_Port, NS_SEG_A_Pin},
    {NS_SEG_B_GPIO_Port, NS_SEG_B_Pin},
    {NS_SEG_C_GPIO_Port, NS_SEG_C_Pin},
    {NS_SEG_D_GPIO_Port, NS_SEG_D_Pin},
    {NS_SEG_E_GPIO_Port, NS_SEG_E_Pin},
    {NS_SEG_F_GPIO_Port, NS_SEG_F_Pin},
    {NS_SEG_G_GPIO_Port, NS_SEG_G_Pin}
};

/* Đông - Tây */
PinRef ewSegPins[7] = {
    {EW_SEG_A_GPIO_Port, EW_SEG_A_Pin},
    {EW_SEG_B_GPIO_Port, EW_SEG_B_Pin},
    {EW_SEG_C_GPIO_Port, EW_SEG_C_Pin},
    {EW_SEG_D_GPIO_Port, EW_SEG_D_Pin},
    {EW_SEG_E_GPIO_Port, EW_SEG_E_Pin},
    {EW_SEG_F_GPIO_Port, EW_SEG_F_Pin},
    {EW_SEG_G_GPIO_Port, EW_SEG_G_Pin}
};

/* Countdown độc lập cho 2 hướng */
int lastNS = -1;
int lastEW = -1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
	void SystemClock_Config(void);
	static void MX_GPIO_Init(void);
	/* USER CODE BEGIN PFP */
	void setDirection(Color c,
	                  GPIO_TypeDef *port,
	                  uint16_t redPin,
	                  uint16_t yellowPin,
	                  uint16_t greenPin);

	void enterPhase(int phaseIndex);

	void display7SEGOn(PinRef* pins, int num);

	void calculateCountdown(uint32_t elapsed,
	                        int phaseIndex,
	                        int *nsSeconds,
	                        int *ewSeconds);

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
  phaseStartTime = HAL_GetTick();
  currentPhase = 0;
  enterPhase(currentPhase);
  lastNS = -1;
  lastEW = -1;
  /* USER CODE END 2 */

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
                          |S_YELLOW_Pin|S_GREEN_Pin|EW_SEG_A_Pin|EW_SEG_B_Pin
                          |E_RED_Pin|E_YELLOW_Pin|E_GREEN_Pin|EW_SEG_C_Pin
                          |EW_SEG_D_Pin|EW_SEG_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, W_RED_Pin|W_YELLOW_Pin|W_GREEN_Pin|EW_SEG_F_Pin
                          |EW_SEG_G_Pin|NS_SEG_A_Pin|NS_SEG_B_Pin|NS_SEG_C_Pin
                          |NS_SEG_D_Pin|NS_SEG_E_Pin|NS_SEG_F_Pin|NS_SEG_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : N_RED_Pin N_YELLOW_Pin N_GREEN_Pin S_RED_Pin
                           S_YELLOW_Pin S_GREEN_Pin EW_SEG_A_Pin EW_SEG_B_Pin
                           E_RED_Pin E_YELLOW_Pin E_GREEN_Pin EW_SEG_C_Pin
                           EW_SEG_D_Pin EW_SEG_E_Pin */
  GPIO_InitStruct.Pin = N_RED_Pin|N_YELLOW_Pin|N_GREEN_Pin|S_RED_Pin
                          |S_YELLOW_Pin|S_GREEN_Pin|EW_SEG_A_Pin|EW_SEG_B_Pin
                          |E_RED_Pin|E_YELLOW_Pin|E_GREEN_Pin|EW_SEG_C_Pin
                          |EW_SEG_D_Pin|EW_SEG_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : W_RED_Pin W_YELLOW_Pin W_GREEN_Pin EW_SEG_F_Pin
                           EW_SEG_G_Pin NS_SEG_A_Pin NS_SEG_B_Pin NS_SEG_C_Pin
                           NS_SEG_D_Pin NS_SEG_E_Pin NS_SEG_F_Pin NS_SEG_G_Pin */
  GPIO_InitStruct.Pin = W_RED_Pin|W_YELLOW_Pin|W_GREEN_Pin|EW_SEG_F_Pin
                          |EW_SEG_G_Pin|NS_SEG_A_Pin|NS_SEG_B_Pin|NS_SEG_C_Pin
                          |NS_SEG_D_Pin|NS_SEG_E_Pin|NS_SEG_F_Pin|NS_SEG_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE BEGIN 4 */

/* ============================================================
 * SET TRAFFIC LIGHT DIRECTION
 * ============================================================ */
void setDirection(Color c,
                  GPIO_TypeDef *port,
                  uint16_t redPin,
                  uint16_t yellowPin,
                  uint16_t greenPin)
{
    HAL_GPIO_WritePin(
        port,
        redPin,
        (c == COLOR_RED) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        port,
        yellowPin,
        (c == COLOR_YELLOW) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        port,
        greenPin,
        (c == COLOR_GREEN) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}


/* ============================================================
 * ENTER NEW PHASE
 * ============================================================ */
void enterPhase(int phaseIndex)
{
    Phase p = phaseTable[phaseIndex];

    /*
     * Bắc
     */
    setDirection(
        p.ns,
        GPIOA,
        N_RED_Pin,
        N_YELLOW_Pin,
        N_GREEN_Pin
    );

    /*
     * Nam
     */
    setDirection(
        p.ns,
        GPIOA,
        S_RED_Pin,
        S_YELLOW_Pin,
        S_GREEN_Pin
    );

    /*
     * Đông
     */
    setDirection(
        p.ew,
        GPIOA,
        E_RED_Pin,
        E_YELLOW_Pin,
        E_GREEN_Pin
    );

    /*
     * Tây
     */
    setDirection(
        p.ew,
        GPIOB,
        W_RED_Pin,
        W_YELLOW_Pin,
        W_GREEN_Pin
    );
}


/* ============================================================
 * DISPLAY ONE DIGIT ON 7-SEGMENT
 * ============================================================ */
void display7SEGOn(PinRef* pins, int num)
{
    if (num < 0 || num > 9)
        return;

    uint8_t pattern = seg_table[num];

    for (int i = 0; i < 7; i++)
    {
        GPIO_PinState state;

        if ((pattern >> i) & 0x01)
        {
            state = GPIO_PIN_SET;
        }
        else
        {
            state = GPIO_PIN_RESET;
        }

        HAL_GPIO_WritePin(
            pins[i].port,
            pins[i].pin,
            state
        );
    }
}


/* ============================================================
 * CALCULATE COUNTDOWN
 *
 * NS and EW are calculated independently.
 *
 * Phase 0:
 *   NS GREEN  = 3 -> 2 -> 1
 *   EW RED    = 5 -> 4 -> 3
 *
 * Phase 1:
 *   NS YELLOW = 2 -> 1
 *   EW RED    = 2 -> 1
 *
 * Phase 2:
 *   NS RED    = 5 -> 4 -> 3
 *   EW GREEN  = 3 -> 2 -> 1
 *
 * Phase 3:
 *   NS RED    = 2 -> 1
 *   EW YELLOW = 2 -> 1
 * ============================================================ */
void calculateCountdown(uint32_t elapsed,
                        int phaseIndex,
                        int *nsSeconds,
                        int *ewSeconds)
{
    uint32_t remaining;

    switch (phaseIndex)
    {
        /* ====================================================
         * PHASE 0
         *
         * NS = GREEN 3s
         * EW = RED   5s
         * ==================================================== */
        case 0:

            /*
             * NS:
             * 3000ms -> 3
             * 2000ms -> 2
             * 1000ms -> 1
             */
            remaining = 3000 - elapsed;

            *nsSeconds = (remaining + 999) / 1000;

            /*
             * EW RED:
             * 5000ms -> 5
             * 4000ms -> 4
             * 3000ms -> 3
             */
            *ewSeconds = 5 - (elapsed / 1000);

            break;


        /* ====================================================
         * PHASE 1
         *
         * NS = YELLOW 2s
         * EW = RED    2s
         * ==================================================== */
        case 1:

            remaining = 2000 - elapsed;

            /*
             * NS:
             * 2 -> 1
             */
            *nsSeconds = (remaining + 999) / 1000;

            /*
             * EW:
             * 2 -> 1
             */
            *ewSeconds = (remaining + 999) / 1000;

            break;


        /* ====================================================
         * PHASE 2
         *
         * NS = RED   5s
         * EW = GREEN 3s
         * ==================================================== */
        case 2:

            /*
             * NS RED:
             *
             * Phase 2 starts after NS has already been
             * RED for 0ms.
             *
             * Therefore:
             * 5 -> 4 -> 3
             */
            *nsSeconds = 5 - (elapsed / 1000);

            /*
             * EW GREEN:
             *
             * 3 -> 2 -> 1
             */
            remaining = 3000 - elapsed;

            *ewSeconds = (remaining + 999) / 1000;

            break;


        /* ====================================================
         * PHASE 3
         *
         * NS = RED    2s
         * EW = YELLOW 2s
         * ==================================================== */
        case 3:

            remaining = 2000 - elapsed;

            /*
             * NS RED:
             * 2 -> 1
             */
            *nsSeconds = (remaining + 999) / 1000;

            /*
             * EW YELLOW:
             * 2 -> 1
             */
            *ewSeconds = (remaining + 999) / 1000;

            break;


        default:

            *nsSeconds = 1;
            *ewSeconds = 1;

            break;
    }


    /* ========================================================
     * LIMIT VALUE FOR 7-SEGMENT
     * ======================================================== */

    if (*nsSeconds < 1)
        *nsSeconds = 1;

    if (*ewSeconds < 1)
        *ewSeconds = 1;

    if (*nsSeconds > 9)
        *nsSeconds = 9;

    if (*ewSeconds > 9)
        *ewSeconds = 9;
}


/* ============================================================
 * UPDATE TRAFFIC LIGHT + COUNTDOWN
 * ============================================================ */
void updateSystem(void)
{
    uint32_t now = HAL_GetTick();

    uint32_t elapsed = now - phaseStartTime;

    uint32_t duration = phaseTable[currentPhase].duration;


    /* ========================================================
     * CHECK PHASE TIMEOUT
     * ======================================================== */

    if (elapsed >= duration)
    {
        /*
         * Chuyển sang phase tiếp theo
         */
        currentPhase = (currentPhase + 1) % 4;

        /*
         * Reset thời gian bắt đầu phase
         */
        phaseStartTime = now;

        /*
         * Cập nhật đèn giao thông
         */
        enterPhase(currentPhase);

        /*
         * Reset countdown
         *
         * Bắt buộc cập nhật lại LED ngay cả khi giá trị
         * trùng với phase trước.
         */
        lastNS = -1;
        lastEW = -1;

        /*
         * Phase mới bắt đầu từ 0ms
         */
        elapsed = 0;
    }


    /* ========================================================
     * CALCULATE COUNTDOWN FOR EACH DIRECTION
     * ======================================================== */

    int nsSecondsLeft;
    int ewSecondsLeft;

    calculateCountdown(
        elapsed,
        currentPhase,
        &nsSecondsLeft,
        &ewSecondsLeft
    );


    /* ========================================================
     * UPDATE NORTH-SOUTH 7-SEGMENT
     * ======================================================== */

    if (nsSecondsLeft != lastNS)
    {
        display7SEGOn(
            nsSegPins,
            nsSecondsLeft
        );

        lastNS = nsSecondsLeft;
    }


    /* ========================================================
     * UPDATE EAST-WEST 7-SEGMENT
     * ======================================================== */

    if (ewSecondsLeft != lastEW)
    {
        display7SEGOn(
            ewSegPins,
            ewSecondsLeft
        );

        lastEW = ewSecondsLeft;
    }
}


/* USER CODE END 4 */
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
