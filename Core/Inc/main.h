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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
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
#define EC11A_Pin GPIO_PIN_13
#define EC11A_GPIO_Port GPIOC
#define EC11A_EXTI_IRQn EXTI15_10_IRQn
#define EC_S_Pin GPIO_PIN_14
#define EC_S_GPIO_Port GPIOC
#define EC_S_EXTI_IRQn EXTI15_10_IRQn
#define EC11B_Pin GPIO_PIN_15
#define EC11B_GPIO_Port GPIOC
#define EC11B_EXTI_IRQn EXTI15_10_IRQn
#define OLED_D1_Pin GPIO_PIN_5
#define OLED_D1_GPIO_Port GPIOB
#define OLED_D0_Pin GPIO_PIN_6
#define OLED_D0_GPIO_Port GPIOB
#define EXTI7_Pin GPIO_PIN_7
#define EXTI7_GPIO_Port GPIOB
#define EXTI7_EXTI_IRQn EXTI9_5_IRQn
#define EXTI9_Pin GPIO_PIN_9
#define EXTI9_GPIO_Port GPIOB
#define EXTI9_EXTI_IRQn EXTI9_5_IRQn

  /* USER CODE BEGIN Private defines */
  void BUCK_CC_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner);
  void BUCK_CV_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner);
  void BOOST_CV_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner);
  void software_start(void);

  extern float32_t phy_V_low;
  extern float32_t phy_I_low;
  extern float32_t phy_V_high;
  extern float32_t phy_I_high;
  extern arm_pid_instance_f32 pid_outer;
  extern arm_pid_instance_f32 pid_inner;

  enum EC_DeBug{
    _phy_setpoint=0,
    _OKp=1,
    _OKi=2,
    _OKd=3,
    _IKp=4,
    _IKi=5,
    _IKd=6,
  };

  extern enum EC_DeBug now_EC_DeBug;
  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
