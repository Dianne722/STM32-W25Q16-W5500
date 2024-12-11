#include "Bootloader.h"
#include "stdint.h"
#include "usart.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
__asm void MSR_MSP(uint32_t addr)
{
    MSR MSP, r0
    BX r14;
}
 
void iap_load_app(void)
{
	APP_FUNC jump2app;//定义一个函数指针
    
    /* 栈顶地址是否合法(这里sram大小为8k) */
    if(((*(uint32_t *)APP_ADDR)&0x2FFFE000) == 0x20000000)
    {
        /* 设置栈指针 */
        MSR_MSP(APP_ADDR);
        /* 获取复位地址 */
		jump2app=(APP_FUNC)*(volatile uint32_t *)(APP_ADDR+4);	
		/* 设置栈指针 */
		__set_MSP(*(volatile uint32_t *)APP_ADDR);
		
	#ifdef BOOTLOADER_LOG	
		//HAL_UART_Transmit(&huart1,(uint8_t*)"Bootloader end load app\r\n",(uint16_t)strlen("Bootloader end load app\r\n"),0xf);
			printf("Bootloader end load app\r\n");
	#endif
 
		/* 跳转之前关闭相应的中断 */
		CLOSE_ALL_INT();
		/* 跳转至APP */
		jump2app();
    }
	
#ifdef BOOTLOADER_LOG
    else
    {
		//HAL_UART_Transmit(&huart1,(uint8_t*)"APP Not Found!\n",(uint16_t)strlen("APP Not Found!\n"),0xf);
			printf("APP Not Found!\n");
    }
#endif
	
}

