/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
#define FLASH_BASE_ADDR    (uint32_t)(0x08000000)
 
//BootLoader   预留10KB的FLASH空间 --- (0x0800 0000 --> 0x0800 27FF)
//Application1 预留20KB的FLASH空间 --- (0x0800 2800 --> 0x0800 77FF)
//Application2 预留20KB的FLASH空间 --- (0x0800 7800 --> 0x0800 C7FF)
 
#define BOOT_BASE_ADDR    FLASH_BASE_ADDR
#define APP1_BASE_ADDR    (uint32_t)(0x08002800)
#define APP2_BASE_ADDR    (uint32_t)(0x08007800)

 
typedef  void (*pFunction)(void);
 
pFunction Boot_Jump_to_App;
 
uint32_t jump_addr;
 
 
#if 0
//初始化用户栈指针汇编程序
__ASM void __set_MSP(uint32_t mainStackPointer)
{
    msr msp, r0
    bx lr
}
#endif
 
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET) // 检查是否是 A8 引脚的中断
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);         // 清除中断标志
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);       // 翻转 B5 的状态
		//					//检查用户代码1的栈顶地址，是否位于0x20000000~0x2001ffff内。
					if (((*(volatile uint32_t*)APP1_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
					{
//							//屏蔽所有中断，防止跳转过程中，中断干扰
//							__disable_irq();
							
							//用户代码的第二个字，为程序开始地址(复位地址)
							jump_addr = *(volatile uint32_t*)(APP1_BASE_ADDR+4);
							Boot_Jump_to_App = (pFunction)jump_addr;
							//初始化用户栈指针
							__set_MSP(*(volatile uint32_t*) APP1_BASE_ADDR);
							//用户程序跳转
							Boot_Jump_to_App();
					}
  }
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET) // 检查是否是 B15 引脚的中断
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);         // 清除中断标志
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);       // 翻转 B3 的状态
		//				 //检查用户代码2的栈顶地址，是否位于0x20000000~0x2001ffff内。
        if (((*(volatile uint32_t*)APP2_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
        {
//            //屏蔽所有中断，防止跳转过程中，中断干扰
//            __disable_irq();
            
            //用户代码的第二个字，为程序开始地址(复位地址)
            jump_addr = *(volatile uint32_t*)(APP2_BASE_ADDR+4);
            Boot_Jump_to_App = (pFunction)jump_addr;
            //初始化用户栈指针
            __set_MSP(*(volatile uint32_t*) APP2_BASE_ADDR);
            //用户程序跳转
            Boot_Jump_to_App();
        }
  }
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
