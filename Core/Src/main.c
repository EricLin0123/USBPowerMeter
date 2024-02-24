/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "screen.h"
#include "imu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// i2c address of ina226
#define INA226_I2C_ADDRESS 0x40
// #define INA226_I2C_ADDRESS 0x45
// register address of ina226
#define INA226_CONFIGURATION 0x00
#define INA226_SHUNT_VOLTAGE 0x01
#define INA226_BUS_VOLTAGE 0x02
#define INA226_POWER 0x03
#define INA226_CURRENT 0x04
#define INA226_CALIBRATION 0x05
#define INA226_MASK_ENABLE 0x06
#define INA226_ALERT_LIMIT0x07
#define R_shunt 0.005f
#define I_LSB 0.001f
#define I_correction 0.4f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t ISR_flag = 1;
uint8_t ISR_flag_prev = 1;
float I_LSB_CORRECTED = I_LSB * I_correction;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void scan_i2c(void);
void INA226_read_buf(uint8_t I2C_ADDRESS, uint8_t addr);
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

  float CAL = 0.00512 / (I_LSB * R_shunt);
  CAL = round(CAL);
  uint8_t CALIBRATION[2];
  CALIBRATION[0] = (uint8_t)((uint16_t)CAL >> 8);   // MSB
  CALIBRATION[1] = (uint8_t)((uint16_t)CAL & 0xFF); // LSB
  char msg[20] = {0};
  sprintf(msg, "value: %d", (uint16_t)CAL);
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM17_Init();
  MX_USART2_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  // scan_i2c();
  OLED_Init();
  OLED_Clear();
  OLED_ShowString(0, 0, (char *)"By", 8);
  for (int i = 0; i < 3; i++)
  {
    OLED_ShowCN_Big(32 * i + 16, 0, i, auth_name2);
    HAL_IWDG_Refresh(&hiwdg);
  }
  HAL_Delay(1500);
  OLED_Clear();
  OLED_ShowString(0, 0, (char *)"To", 8);
  OLED_ShowName(16, 0, member_name1);
  OLED_ShowName(80, 0, member_name2);
  OLED_ShowName(16, 2, member_name3);
  OLED_ShowName(80, 2, member_name4);
  HAL_IWDG_Refresh(&hiwdg);
  HAL_Delay(1500);
  OLED_Clear();

  while (ADXL345_Init() == HAL_ERROR)
  {
    HAL_IWDG_Refresh(&hiwdg);
  }
  HAL_IWDG_Refresh(&hiwdg);
  OLED_Clear();

  if (HAL_I2C_Mem_Write(&hi2c1, INA226_I2C_ADDRESS << 1, INA226_CALIBRATION, 1, CALIBRATION, 2, 1000) == HAL_OK)
  {
    OLED_ShowString(0, 0, (char *)"ADC calibrated", 10);
    OLED_ShowString(0, 1, msg, 10);
    HAL_Delay(500);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == 1)
  {
    OLED_ShowString(0, 0, (char *)"NRST SOURCE   ", 10);
    OLED_ShowString(0, 1, (char *)"IWDG reset", 10);
    __HAL_RCC_CLEAR_RESET_FLAGS();
    HAL_Delay(500);
    HAL_IWDG_Refresh(&hiwdg);
  }
  else
  {
    OLED_ShowString(0, 0, (char *)"NRST SOURCE   ", 10);
    OLED_ShowString(0, 1, (char *)"NRST reset    ", 10);
    __HAL_RCC_CLEAR_RESET_FLAGS();
    HAL_Delay(500);
    HAL_IWDG_Refresh(&hiwdg);
  }
  for (uint8_t i = 0; i < 12; i++)
  {
    OLED_ShowEN(8 * i, 0, i, Hzk2);
  }
  HAL_IWDG_Refresh(&hiwdg);
  ISR_flag = 1;
  ISR_flag_prev = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (ISR_flag == 2)
    {
      if (ISR_flag_prev == 1)
      {
        OLED_Clear();
        for (uint8_t i = 0; i < 13; i++)
        {
          OLED_ShowEN(8 * i, 0, i, imu_title);
        }
        ISR_flag_prev = 2;
      }
      ADXL345_Test();
    }
    else if (ISR_flag == 1)
    {
      if (ISR_flag_prev == 2)
      {
        OLED_Clear();
        for (uint8_t i = 0; i < 12; i++)
        {
          OLED_ShowEN(8 * i, 0, i, Hzk2);
        }
        ISR_flag_prev = 1;
      }

      INA226_read_buf(INA226_I2C_ADDRESS, INA226_BUS_VOLTAGE);
      INA226_read_buf(INA226_I2C_ADDRESS, INA226_CURRENT);
      INA226_read_buf(INA226_I2C_ADDRESS, INA226_POWER);
      ADXL345_Probe();
    }

    ADXL345_Rread(ADXL345_INT_SOURCE, (uint8_t *)msg);

    HAL_IWDG_Refresh(&hiwdg);
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c1)
{
  HAL_I2C_MspDeInit(hi2c1);
  HAL_I2C_MspInit(hi2c1);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_11)
  {
    if (ISR_flag == 1)
    {
      ISR_flag = 2;
    }
    else if (ISR_flag == 2)
    {
      ISR_flag = 1;
    }
  }
}

// ina226 read function
void INA226_read_buf(uint8_t I2C_ADDRESS, uint8_t addr)
{
  char ReadBuffer[2] = {0};
  char msg[] = "";
  static float V_shunt, V, I, P;
  if (HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS << 1, addr, 1, (uint8_t *)ReadBuffer, 2, 1000) == HAL_OK)
  {
    if (addr == INA226_SHUNT_VOLTAGE)
    {
      uint16_t v = (uint16_t)(ReadBuffer[0] << 8 | ReadBuffer[1]);
      // convert from 2' complement to decimal
      v -= 1;
      v = ~v;
      V_shunt = (float)(v * 2.5E-6);
      sprintf(msg, "VS: %f%c", V_shunt, '\0');
      OLED_ShowString(0, 2, msg, 8);
    }
    else
    {
      uint16_t res = (uint16_t)(ReadBuffer[0] << 8 | ReadBuffer[1]);
      if (addr == INA226_CURRENT)
      {
        I = (float)(res * I_LSB_CORRECTED);
        sprintf(msg, "I:%.3f%c", I, '\0');
        OLED_ShowString(0, 3, msg, 8);
      }
      else if (addr == INA226_BUS_VOLTAGE)
      {
        V = (float)(res * 1.25E-3);
        sprintf(msg, "V:%.3f%c", V, '\0');
        OLED_ShowString(0, 2, msg, 8);
      }
      else if (addr == INA226_POWER)
      {
        P = (float)(res * 25 * I_LSB_CORRECTED);
        sprintf(msg, "P:%.3f%c", P, '\0');
        OLED_ShowString(64, 3, msg, 8);
      }
    }
  }
  else
  {
    HAL_UART_Transmit(&huart2, "Fail to read\r\n", 14, 1000);
  }
}

void scan_i2c(void)
{
  char WriteBuffer[20] = {0};
  for (uint8_t i = 0; i < 128; i++)
  {
    if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 3, 5) == HAL_OK)
    {
      // We got an ack
      sprintf(WriteBuffer, "0x%X", i);
      HAL_UART_Transmit(&huart2, (uint8_t *)WriteBuffer, 5, 1000);
    }
    else
    {
      HAL_UART_Transmit(&huart2, "--", 2, 1000);
    }

    if (i > 0 && (i + 1) % 16 == 0)
      HAL_UART_Transmit(&huart2, "\r\n", 2, 1000);
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

#ifdef  USE_FULL_ASSERT
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
