#include "bootloader.h"
 
#include "stdio.h"
#include "interrupt.h"
extern struct keys key[];
 
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
 
/**
  * @brief  BootLoader程序
  * @param  None
  * @retval None
  */
void BootLoader_Code(void)
{
        
    //扫描按键状态，根据按键跳转到相应的APP程序
    while(1)
    {
			if(key[0].single_flag==1)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
//					//检查用户代码1的栈顶地址，是否位于0x20000000~0x2001ffff内。
//					if (((*(volatile uint32_t*)APP1_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
//					{
//						//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_SET);
//							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
//							//屏蔽所有中断，防止跳转过程中，中断干扰
//							__disable_irq();
//							
//							//用户代码的第二个字，为程序开始地址(复位地址)
//							jump_addr = *(volatile uint32_t*)(APP1_BASE_ADDR+4);
//							Boot_Jump_to_App = (pFunction)jump_addr;
//							//初始化用户栈指针
//							__set_MSP(*(volatile uint32_t*) APP1_BASE_ADDR);
//							//用户程序跳转
//							Boot_Jump_to_App();
//					}
					//做出相应动作
					 key[0].single_flag=0;
				}
			if(key[1].single_flag==1)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
//				 //检查用户代码2的栈顶地址，是否位于0x20000000~0x2001ffff内。
//        if (((*(volatile uint32_t*)APP2_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
//        {
//					//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_SET);
//           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
//            //屏蔽所有中断，防止跳转过程中，中断干扰
//            __disable_irq();
//            
//            //用户代码的第二个字，为程序开始地址(复位地址)
//            jump_addr = *(volatile uint32_t*)(APP2_BASE_ADDR+4);
//            Boot_Jump_to_App = (pFunction)jump_addr;
//            //初始化用户栈指针
//            __set_MSP(*(volatile uint32_t*) APP2_BASE_ADDR);
//            //用户程序跳转
//            Boot_Jump_to_App();
//        }
					//做出相应动作	
					key[1].single_flag=0;
			}
    }
}



 


 