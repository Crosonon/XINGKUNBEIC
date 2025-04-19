/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define Joy1_Pin GPIO_PIN_1
#define Joy1_GPIO_Port GPIOA
#define Diastimeter_Echo_Pin GPIO_PIN_4
#define Diastimeter_Echo_GPIO_Port GPIOA
#define Diastimeter_Trig_Pin GPIO_PIN_5
#define Diastimeter_Trig_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOA
#define Joy2_Pin GPIO_PIN_0
#define Joy2_GPIO_Port GPIOB
#define Battery_Pin GPIO_PIN_1
#define Battery_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_10
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_11
#define OLED_SDA_GPIO_Port GPIOB
#define Motor_1L_En_Pin GPIO_PIN_12
#define Motor_1L_En_GPIO_Port GPIOB
#define Motor_2H_En_Pin GPIO_PIN_13
#define Motor_2H_En_GPIO_Port GPIOB
#define Motor_1L_Dir_Pin GPIO_PIN_14
#define Motor_1L_Dir_GPIO_Port GPIOB
#define Motor_2H_Dir_Pin GPIO_PIN_15
#define Motor_2H_Dir_GPIO_Port GPIOB
#define Motor_1L_Step_Pin GPIO_PIN_8
#define Motor_1L_Step_GPIO_Port GPIOA
#define Motor_2H_Step_Pin GPIO_PIN_9
#define Motor_2H_Step_GPIO_Port GPIOA
#define Key3_Pin GPIO_PIN_10
#define Key3_GPIO_Port GPIOA
#define Key4_Pin GPIO_PIN_11
#define Key4_GPIO_Port GPIOA
#define Key5_Pin GPIO_PIN_12
#define Key5_GPIO_Port GPIOA
#define Laser_Pin GPIO_PIN_3
#define Laser_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_5
#define SW1_GPIO_Port GPIOB
#define Key1_Pin GPIO_PIN_8
#define Key1_GPIO_Port GPIOB
#define Key2_Pin GPIO_PIN_9
#define Key2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
