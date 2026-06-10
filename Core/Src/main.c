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
#include "adc.h"
#include "dma.h"
#include "hrtim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "OLED.h"
#include "MY_OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define inner_upper_limit                           59400.0f
#define inner_lower_limit                           600.0f
#define BUCK_CC_upper_limit                         18.0f
#define BUCK_CC_lower_limit                         12.8f
#define BUCK_CV_upper_limit                         2.1f
#define BUCK_CV_lower_limit                         0.0f
#define BOOST_CV_upper_limit                        1.2f
#define BOOST_CV_lower_limit                        (-1.0f)
#define software_step_size                          600.0f
#define software_start_digital_setpoint             30000.0f
#define phy_calc_conv_voltage                       1.0/16.0f
#define phy_calc_conv_current                       1.0/16.0f

enum {
  BOOST_CV=0,
  BUCK_CC=1,
  BUCK_CV=2,
  IDLE=3,
}control_mode=BUCK_CV;

enum EC_DeBug now_EC_DeBug=_phy_setpoint;

float32_t phy_setpoint=15.0f;
float32_t calc_setpoint=0;
float32_t calc_measurement[4]={0.0f,0.0f,0.0f,0.0f};
float32_t phy_V_low,phy_I_low,phy_V_high,phy_I_high;
float32_t pid_memory[3];
float32_t debug_value[2]={0.0f,0.0f};

HRTIM_CompareCfgTypeDef ACMP1_T={0};

uint16_t ADC_value[64];//4个通道，16个数据取平均滤波
//const uint8_t buff_size=16;

float32_t outer_upper_limit=BUCK_CV_upper_limit;
float32_t outer_lower_limit=BUCK_CV_lower_limit;

//const float32_t conv_gain=1.0;
//const float32_t phy_calc_conv=conv_gain/(float32_t)buff_size;
float32_t phy_calc_conv=phy_calc_conv_voltage;
//const float32_t Ts=5e-5f;


arm_pid_instance_f32 pid_outer,pid_inner;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_HRTIM1_Init();
  /* USER CODE BEGIN 2 */
  //HAL_Delay(200);
  OLED_Init();

  HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_value,sizeof(ADC_value)/sizeof(uint16_t));

  switch (control_mode) {
    case BUCK_CC:
      BUCK_CC_init(&pid_outer,&pid_inner);
      break;
    case BUCK_CV:
      BUCK_CV_init(&pid_outer,&pid_inner);
      break;
    case BOOST_CV:
      BOOST_CV_init(&pid_outer,&pid_inner);
      break;
    default:
      break;
  }

  HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);//开启定时器

  software_start();//软件软启动

  //__HAL_HRTIM_TIMER_ENABLE_IT(&hhrtim1,HRTIM_TIMERINDEX_TIMER_C,HRTIM_TIM_IT_UPD);//开启更新中断,开启PID

  DeBug_interface_head();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch (control_mode) {
      case BUCK_CC:
        phy_V_low=calc_measurement[2]*phy_calc_conv_voltage;
        phy_I_low=calc_measurement[3]*phy_calc_conv_current;
        BUCK_CC_interface_main();
        break;
      case BUCK_CV:
        phy_V_low=calc_measurement[2]*phy_calc_conv_voltage;
        phy_I_low=calc_measurement[3]*phy_calc_conv_current;
        BUCK_CV_interface_main();
        break;
      case BOOST_CV:
        phy_V_high=calc_measurement[0]*phy_calc_conv_voltage;
        phy_I_high=calc_measurement[1]*phy_calc_conv_current;
        BOOST_CV_interface_main();
        break;
      default:
        break;
    }
    DeBug_interface_main();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 75;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void BUCK_CC_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner){
  control_mode=BUCK_CC;
  phy_setpoint=0.0f;
  calc_setpoint=0;
  phy_calc_conv=phy_calc_conv_current;

  pid_outer->Kp=0.0f;
  pid_outer->Ki=0.0f;
  pid_outer->Kd=0.0f;
  pid_outer->state[0]=0.0f;
  pid_outer->state[1]=0.0f;
  pid_outer->state[2]=0.0f;
  //pid_outer->A0=pid_outer->Kp+pid_outer->Ki*Ts+pid_outer->Kd/Ts;
  //pid_outer->A1=-pid_outer->Kp-2.0f*pid_outer->Kd/Ts;
  //pid_outer->A2=pid_outer->Kd/Ts;

  pid_inner->Kp=0.0f;
  pid_inner->Ki=0.0f;
  pid_inner->Kd=0.0f;
  pid_inner->state[0]=0.0f;
  pid_inner->state[1]=0.0f;
  pid_inner->state[2]=0.0f;
  //pid_inner->A0=pid_inner->Kp+pid_inner->Ki*Ts+pid_inner->Kd/Ts;
  //pid_inner->A1=-pid_inner->Kp-2.0f*pid_inner->Kd/Ts;
  //pid_inner->A2=pid_inner->Kd/Ts;

  outer_upper_limit=BUCK_CC_upper_limit;
  outer_lower_limit=BUCK_CC_lower_limit;

  arm_pid_init_f32(pid_outer,0);
  arm_pid_init_f32(pid_inner,0);

  BUCK_CC_interface_head();
}

void BUCK_CV_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner){
  control_mode=BUCK_CV;
  phy_setpoint=0.0f;
  calc_setpoint=0;
  phy_calc_conv=phy_calc_conv_voltage;


  pid_outer->Kp=0.0f;
  pid_outer->Ki=0.0f;
  pid_outer->Kd=0.0f;
  pid_outer->state[0]=0.0f;
  pid_outer->state[1]=0.0f;
  pid_outer->state[2]=0.0f;
  //pid_outer->A0=pid_outer->Kp+pid_outer->Ki*Ts+pid_outer->Kd/Ts;
  //pid_outer->A1=-pid_outer->Kp-2.0f*pid_outer->Kd/Ts;
  //pid_outer->A2=pid_outer->Kd/Ts;

  pid_inner->Kp=0.0f;
  pid_inner->Ki=0.0f;
  pid_inner->Kd=0.0f;
  pid_inner->state[0]=0.0f;
  pid_inner->state[1]=0.0f;
  pid_inner->state[2]=0.0f;
  //pid_inner->A0=pid_inner->Kp+pid_inner->Ki*Ts+pid_inner->Kd/Ts;
  //pid_inner->A1=-pid_inner->Kp-2.0f*pid_inner->Kd/Ts;
  //pid_inner->A2=pid_inner->Kd/Ts;

  outer_upper_limit=BUCK_CV_upper_limit;
  outer_lower_limit=BUCK_CV_lower_limit;

  arm_pid_init_f32(pid_outer,0);
  arm_pid_init_f32(pid_inner,0);

  BUCK_CV_interface_head();
}

void BOOST_CV_init(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner){
  control_mode=BOOST_CV;
  phy_setpoint=0.0f;
  calc_setpoint=0;
  phy_calc_conv=phy_calc_conv_voltage;

  pid_outer->Kp=0.0f;
  pid_outer->Ki=0.0f;
  pid_outer->Kd=0.0f;
  pid_outer->state[0]=0.0f;
  pid_outer->state[1]=0.0f;
  pid_outer->state[2]=0.0f;
  //pid_outer->A0=pid_outer->Kp+pid_outer->Ki*Ts+pid_outer->Kd/Ts;
  //pid_outer->A1=-pid_outer->Kp-2.0f*pid_outer->Kd/Ts;
  //pid_outer->A2=pid_outer->Kd/Ts;

  pid_inner->Kp=0.0f;
  pid_inner->Ki=0.0f;
  pid_inner->Kd=0.0f;
  pid_inner->state[0]=0.0f;
  pid_inner->state[1]=0.0f;
  pid_inner->state[2]=0.0f;
  //pid_inner->A0=pid_inner->Kp+pid_inner->Ki*Ts+pid_inner->Kd/Ts;
  //pid_inner->A1=-pid_inner->Kp-2.0f*pid_inner->Kd/Ts;
  //pid_inner->A2=pid_inner->Kd/Ts;

  outer_upper_limit=BOOST_CV_upper_limit;
  outer_lower_limit=BOOST_CV_lower_limit;

  arm_pid_init_f32(pid_outer,0);
  arm_pid_init_f32(pid_inner,0);

  BOOST_CV_interface_head();
}

void software_start(void) {
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1|HRTIM_OUTPUT_TC2);//开启通道输出

  for (int i=0;i<=software_start_digital_setpoint;i+=software_step_size)
  {
    ACMP1_T.CompareValue=i;
    HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, &ACMP1_T);
    HAL_Delay(2);
  }
}

uint32_t PID_Compute(arm_pid_instance_f32 *pid_outer,arm_pid_instance_f32 *pid_inner,const float32_t error_outer,
  float32_t inner_measurement){
  arm_copy_f32(pid_outer->state,pid_memory,3);
  float32_t error_inner=0;
  float32_t output = arm_pid_f32(pid_outer, error_outer);
  debug_value[0]=output;

  if (output > outer_upper_limit)
  {
    arm_copy_f32(pid_memory,pid_outer->state,3);
    error_inner=outer_upper_limit-inner_measurement;
  }
  if (output < outer_lower_limit)
  {
    arm_copy_f32(pid_memory,pid_outer->state,3);
    error_inner=outer_lower_limit-inner_measurement;
  }
  error_inner=output-inner_measurement;
  arm_copy_f32(pid_inner->state,pid_memory,3);
  output = arm_pid_f32(pid_inner, error_inner);
  debug_value[1]=output;

  if (output > inner_upper_limit)
  {
    arm_copy_f32(pid_memory,pid_inner->state,3);
    return inner_upper_limit;
  }
  if (output < inner_lower_limit)
  {
    arm_copy_f32(pid_memory,pid_inner->state,3);
    return inner_lower_limit;
  }
  return output;
}

void HAL_HRTIM_RegistersUpdateCallback(HRTIM_HandleTypeDef *hhrtim,uint32_t TimerIdx) {
  static uint8_t count = 0;
  static uint32_t out=0;
  if (TimerIdx == HRTIM_TIMERINDEX_TIMER_C)
    {
      count++;
      if (count%2==0)
      {
        if (control_mode)
        {
          calc_measurement[0]=ADC_value[0]+ADC_value[4]+ADC_value[8]+ADC_value[12]+ADC_value[16]
          +ADC_value[20]+ADC_value[24]+ADC_value[28]+ADC_value[32]+ADC_value[36]+ADC_value[40]
          +ADC_value[44]+ADC_value[48]+ADC_value[52]+ADC_value[56]+ADC_value[60];
          calc_measurement[1]=ADC_value[1]+ADC_value[5]+ADC_value[9]+ADC_value[13]+ADC_value[17]
          +ADC_value[21]+ADC_value[25]+ADC_value[29]+ADC_value[33]+ADC_value[37]+ADC_value[41]
          +ADC_value[45]+ADC_value[49]+ADC_value[53]+ADC_value[57]+ADC_value[61];
        }
        else {
          calc_measurement[2]=ADC_value[2]+ADC_value[6]+ADC_value[10]+ADC_value[14]+ADC_value[18]
          +ADC_value[22]+ADC_value[26]+ADC_value[30]+ADC_value[34]+ADC_value[38]+ADC_value[42]
          +ADC_value[46]+ADC_value[50]+ADC_value[54]+ADC_value[58]+ADC_value[62];
          calc_measurement[3]=ADC_value[3]+ADC_value[7]+ADC_value[11]+ADC_value[15]+ADC_value[19]
          +ADC_value[23]+ADC_value[27]+ADC_value[31]+ADC_value[35]+ADC_value[39]+ADC_value[43]
          +ADC_value[47]+ADC_value[51]+ADC_value[55]+ADC_value[59]+ADC_value[63];
        }
        switch (control_mode) {
          case BOOST_CV:
            out=PID_Compute(&pid_outer,&pid_inner,calc_setpoint-calc_measurement[2],
              calc_measurement[3]);
            ACMP1_T.CompareValue=out;
            HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, &ACMP1_T);
            break;
          case BUCK_CC:
            out=PID_Compute(&pid_outer,&pid_inner,calc_setpoint-calc_measurement[1],
              calc_measurement[0]);
            ACMP1_T.CompareValue=out;
            HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, &ACMP1_T);
            break;
          case BUCK_CV:
            out=PID_Compute(&pid_outer,&pid_inner,calc_setpoint-calc_measurement[0],
              calc_measurement[1]);
            ACMP1_T.CompareValue=out;
            HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, &ACMP1_T);
            break;
          default:
            break;
        }
      }

    }

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin==EXTI7_Pin) {
    control_mode=(control_mode+1)%4;
    switch (control_mode) {
      case BOOST_CV:
        BOOST_CV_init(&pid_outer, &pid_inner);
        break;
      case BUCK_CC:
        BUCK_CC_init(&pid_outer, &pid_inner);
        break;
      case BUCK_CV:
        BUCK_CV_init(&pid_outer, &pid_inner);
        break;
      default:
        break;
    }
  }
  if (GPIO_Pin==EXTI9_Pin)
  {

  }
  if (GPIO_Pin==EC_S_Pin)
  {
    now_EC_DeBug=(now_EC_DeBug+1)%7;
  }
  if (GPIO_Pin==EC11A_Pin)
  {
    if (HAL_GPIO_ReadPin(EC11A_GPIO_Port,EC11A_Pin)==0) {
      if (HAL_GPIO_ReadPin(EC11B_GPIO_Port,EC11B_Pin)==0) {
        switch (now_EC_DeBug) {
          case _phy_setpoint:
            if (phy_setpoint<outer_upper_limit) {
              phy_setpoint+=0.01f;
              calc_setpoint=phy_setpoint/phy_calc_conv;
            }
            break;
          case _OKp:
            pid_outer.Kp+=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            pid_outer.A1=-pid_outer.Kp-2.0f*pid_outer.Kd;
            break;
          case _OKi:
            pid_outer.Ki+=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            break;
          case _OKd:
            pid_outer.Kd+=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            pid_outer.A1=-pid_outer.Kp-2.0f*pid_outer.Kd;
            pid_outer.A2=pid_outer.Kd;
            break;
          case _IKp:
            pid_inner.Kp+=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            pid_inner.A1=-pid_inner.Kp-2.0f*pid_inner.Kd;
            break;
          case _IKi:
            pid_inner.Ki+=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            break;
          case _IKd:
            pid_inner.Kd+=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            pid_inner.A1=-pid_inner.Kp-2.0f*pid_inner.Kd;
            pid_inner.A2=pid_inner.Kd;
            break;
          default:
            break;
        }
      }
    }
  }
  if (GPIO_Pin==EC11B_Pin)
  {
    if (HAL_GPIO_ReadPin(EC11B_GPIO_Port,EC11B_Pin)==0) {
      if (HAL_GPIO_ReadPin(EC11A_GPIO_Port,EC11A_Pin)==0) {
        switch (now_EC_DeBug) {
          case _phy_setpoint:
            if (phy_setpoint>outer_lower_limit) {
              phy_setpoint-=0.01f;
              calc_setpoint=phy_setpoint/phy_calc_conv;
            }
            break;
          case _OKp:
            pid_outer.Kp-=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            pid_outer.A1=-pid_outer.Kp-2.0f*pid_outer.Kd;
            break;
          case _OKi:
            pid_outer.Ki-=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            break;
          case _OKd:
            pid_outer.Kd-=0.1f;
            pid_outer.A0=pid_outer.Kp+pid_outer.Ki+pid_outer.Kd;
            pid_outer.A1=-pid_outer.Kp-2.0f*pid_outer.Kd;
            pid_outer.A2=pid_outer.Kd;
            break;
          case _IKp:
            pid_inner.Kp-=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            pid_inner.A1=-pid_inner.Kp-2.0f*pid_inner.Kd;
            break;
          case _IKi:
            pid_inner.Ki-=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            break;
          case _IKd:
            pid_inner.Kd-=0.1f;
            pid_inner.A0=pid_inner.Kp+pid_inner.Ki+pid_inner.Kd;
            pid_inner.A1=-pid_inner.Kp-2.0f*pid_inner.Kd;
            pid_inner.A2=pid_inner.Kd;
            break;
          default:
            break;
        }
      }
    }
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
