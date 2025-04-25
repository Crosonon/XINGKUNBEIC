/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "MOTOR.h"
#include "KEY.h"
#include "Menu.h"
#include "JOYSTICK.h"
#include "Coordinate.h"
#include "queue.h"
#include "Control.h"
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
uint8_t rx_data;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//uart1回调，收集k210给的数据，存储到x和y
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
  static uint8_t tmp_data;  //临时存储
  
  if (huart->Instance == USART1)
  {
    tmp_data = rx_data;  //获取数据
    
    switch(uart1_rx_state) 
    {
      case 0:
        if(tmp_data == 0xff) 
        {
          uart1_rx_state = 1;
          uart1_rx_length = 0;
          uart1_rx_flag = 0;
        }
        break;
        
      case 1:
        if(tmp_data == 0x00)
        {
          uart1_rx_state = 2;
          uart1_rx_mode = 0;
        }
        else if (tmp_data == 0x01)
        {
          uart1_rx_state = 2;
          uart1_rx_mode = 1;
        }
        break;

      case 2:  //接收数据体
        if(tmp_data == 0xfe) //end
        {
          uart1_rx_state = 0;
          uart1_rx_buffer[uart1_rx_length] = '\0';
          uart1_rx_flag = 1;
          if (uart1_rx_mode == 0)//写入pixnow
          {
            laser.Now_Pix.x = (uart1_rx_buffer[0]<<8) | (uart1_rx_buffer[1]);
            laser.Now_Pix.y = (uart1_rx_buffer[2]<<8) | (uart1_rx_buffer[3]);
          }
          else if (uart1_rx_mode == 1)//写入pixset
          {
            // sys_set.Cam_Point.x = (uart1_rx_buffer[0]<<8) | (uart1_rx_buffer[1]);
            // sys_set.Cam_Point.y = (uart1_rx_buffer[2]<<8) | (uart1_rx_buffer[3]);
          }
          uart1_rx_mode = 0;
        } 
        else 
        {
          if(uart1_rx_length < RX_BUFFER_SIZE-1) 
          { 
            uart1_rx_buffer[uart1_rx_length++] = tmp_data;
          }
        }
        break;
    }

    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim -> Instance == TIM1)
  {
    //10ms
    /******************************************************************************************************** */
    //t是调试代码
    //更新Del_mm(如果set或者now更新了，才会更新del，否则del在这里不更新，只在每次移动的时候改变)
    // uint8_t t = CheckUpdate_Del();
    // if (t) OLED_ShowNum(3,13,8,1);


    //以上
    static uint16_t tim_i = 0;
    if(tim_i == 10)
    {
      tim_i = 0;

      static Pixel_Point last_Set = {0,0};
      static Pixel_Point last_Now = {0,0};
      
      uint8_t Update = (last_Now.x != laser.Now_Pix.x || last_Now.y != laser.Now_Pix.y /*|| last_Set.x != laser.Set_Pix.x || last_Set.y != laser.Set_Pix.y*/);
      if (Update)
      {
          Pixel_Point Del_Pix = {
              .x = laser.Set_Pix.x - laser.Now_Pix.x,
              .y = laser.Set_Pix.y - laser.Now_Pix.y
          };
  
          OLED_ShowNum(2,1,Del_Pix.x,4);
          OLED_ShowNum(2,5,Del_Pix.y,4);
  
          laser.Del_mm = Pixel_to_mm(Del_Pix);
          
  
          last_Set.x = laser.Set_Pix.x;
          last_Set.y = laser.Set_Pix.y;
          last_Now.x = laser.Now_Pix.x;
          last_Now.y = laser.Now_Pix.y;
      }
  
      // if(Update == 0) OLED_ShowNum(2,11,0,1);
      if(Update == 1) OLED_ShowNum(2,11,1,1);
      // HAL_Delay(100);//写了这句直接oled不刷新了
    }
    else tim_i ++;
  }
  if (htim -> Instance == TIM3)
  {
    //10ms
    //如果队列已经走完，则直接退出
    // if(sys_set.Flag.End == 1)return;

    //调取下一个点（如果刚设置模式，则此时arrive为1，会取刚刚入队的点）
    if (sys_set.Flag.Arrive)
    {
      //调用下一个点到set，如果65535则结束
      Pixel_Point point = Point_Queue_Dequeue(&(sys_set.Target_Point));
      if (point.x > 1000 || point.x == 0)
      {
        sys_set.Flag.End = 1;
        Control_SetMode(Mode_Joystick);
      }
      else
      {
        laser.Set_Pix = point;
        sys_set.Flag.Arrive = 0;
      }
    }

    //更新Del_mm(如果set或者now更新了，才会更新del，否则del在这里不更新，只在每次移动的时候改变)
    CheckUpdate_Del();
    // Motor_Lock_Check();

    // 给两个电机设定方向，并检测是否到达,如果到达了会给两个电机置stop，下面不会动
    //更新到达情况和方向,如果到了置1,没到置0
    sys_set.Flag.Arrive = !Motor_Dir_Set(&motor_1L, &motor_2H, laser.Del_mm);
    // 如果是摇杆模式，直接设定还没到达，并设定方向
    if(Current_Mode == Mode_Joystick)
    {
      // sys_set.Flag.Arrive = 0;
      motor_1L.Dir = Joy_Get_X();
      motor_2H.Dir = Joy_Get_Y();
    }

    //判断是否走，走一步,并更新电机的位置
    //调试需要先注释了
    Motor_Update_Position(&motor_1L, &laser.Del_mm.x, Motor_Step_Dis(motor_1L, laser));
    Motor_Update_Position(&motor_2H, &laser.Del_mm.y, Motor_Step_Dis(motor_2H, laser));

  }
}
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
  MX_TIM1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcData, 3);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);
  OLED_Init();
  Motor_Init();
  Menu_Init();


  Point_Queue_Init(&(sys_set.Cam_Point));


    /*无传入的调试信息如下*/

    //用于无初始化的调试
    //{{290,20},{30,20},{23,216},{294,218}};
    sys_set.Calib_Point[0].x = 290;
    sys_set.Calib_Point[0].y = 20;
    sys_set.Calib_Point[1].x = 30;
    sys_set.Calib_Point[1].y = 20;
    sys_set.Calib_Point[2].x = 23;
    sys_set.Calib_Point[2].y = 216;
    sys_set.Calib_Point[3].x = 294;
    sys_set.Calib_Point[3].y = 218;

    Coordinate_Init();

    laser.Now_Pix.x = 100;
    laser.Now_Pix.y = 200;
    //
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    KEY_Act(KEY_GetNum());
    ADC_Update();
    Menu_PageShow();



    uint8_t t = CheckUpdate_Del();
    if (t) OLED_ShowNum(3,13,t,1);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
