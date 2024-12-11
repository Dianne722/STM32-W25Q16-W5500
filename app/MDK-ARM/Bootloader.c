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
	APP_FUNC jump2app;//����һ������ָ��
    
    /* ջ����ַ�Ƿ�Ϸ�(����sram��СΪ8k) */
    if(((*(uint32_t *)APP_ADDR)&0x2FFFE000) == 0x20000000)
    {
        /* ����ջָ�� */
        MSR_MSP(APP_ADDR);
        /* ��ȡ��λ��ַ */
		jump2app=(APP_FUNC)*(volatile uint32_t *)(APP_ADDR+4);	
		/* ����ջָ�� */
		__set_MSP(*(volatile uint32_t *)APP_ADDR);
		
	#ifdef BOOTLOADER_LOG	
		//HAL_UART_Transmit(&huart1,(uint8_t*)"Bootloader end load app\r\n",(uint16_t)strlen("Bootloader end load app\r\n"),0xf);
			printf("Bootloader end load app\r\n");
	#endif
 
		/* ��ת֮ǰ�ر���Ӧ���ж� */
		CLOSE_ALL_INT();
		/* ��ת��APP */
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

