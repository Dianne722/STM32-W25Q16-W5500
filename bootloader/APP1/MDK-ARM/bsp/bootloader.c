#include "bootloader.h"
 
#include "stdio.h"
#include "interrupt.h"
extern struct keys key[];
 
typedef  void (*pFunction)(void);
 
pFunction Boot_Jump_to_App;
 
uint32_t jump_addr;
 
 
#if 0
//��ʼ���û�ջָ�������
__ASM void __set_MSP(uint32_t mainStackPointer)
{
    msr msp, r0
    bx lr
}
#endif
 
/**
  * @brief  BootLoader����
  * @param  None
  * @retval None
  */
void BootLoader_Code(void)
{
        
    //ɨ�谴��״̬�����ݰ�����ת����Ӧ��APP����
    while(1)
    {
			if(key[0].single_flag==1)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
//					//����û�����1��ջ����ַ���Ƿ�λ��0x20000000~0x2001ffff�ڡ�
//					if (((*(volatile uint32_t*)APP1_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
//					{
//						//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_SET);
//							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
//							//���������жϣ���ֹ��ת�����У��жϸ���
//							__disable_irq();
//							
//							//�û�����ĵڶ����֣�Ϊ����ʼ��ַ(��λ��ַ)
//							jump_addr = *(volatile uint32_t*)(APP1_BASE_ADDR+4);
//							Boot_Jump_to_App = (pFunction)jump_addr;
//							//��ʼ���û�ջָ��
//							__set_MSP(*(volatile uint32_t*) APP1_BASE_ADDR);
//							//�û�������ת
//							Boot_Jump_to_App();
//					}
					//������Ӧ����
					 key[0].single_flag=0;
				}
			if(key[1].single_flag==1)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
//				 //����û�����2��ջ����ַ���Ƿ�λ��0x20000000~0x2001ffff�ڡ�
//        if (((*(volatile uint32_t*)APP2_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000)
//        {
//					//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_5, GPIO_PIN_SET);
//           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
//            //���������жϣ���ֹ��ת�����У��жϸ���
//            __disable_irq();
//            
//            //�û�����ĵڶ����֣�Ϊ����ʼ��ַ(��λ��ַ)
//            jump_addr = *(volatile uint32_t*)(APP2_BASE_ADDR+4);
//            Boot_Jump_to_App = (pFunction)jump_addr;
//            //��ʼ���û�ջָ��
//            __set_MSP(*(volatile uint32_t*) APP2_BASE_ADDR);
//            //�û�������ת
//            Boot_Jump_to_App();
//        }
					//������Ӧ����	
					key[1].single_flag=0;
			}
    }
}



 


 