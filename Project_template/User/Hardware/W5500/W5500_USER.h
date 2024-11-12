#ifndef __W5500_USER_H
#define __W5500_USER_H	
#include "W5500.h"

extern unsigned int W5500_Send_Delay_Counter;		//W5500������ʱ��������(ms)

void RCC_Configuration(void);						//����ϵͳʱ��Ϊ72MHZ(������Ը�����Ҫ��)
void NVIC_Configuration(void);					//STM32�ж�������������
void Timer2_Init_Config(void);					//Timer2��ʼ������
void System_Initialization(void);				//STM32ϵͳ��ʼ������(��ʼ��STM32ʱ�Ӽ�����)
void Delay(unsigned int d);							//��ʱ����(ms)

void W5500_Initialization(void);				//W5500��ʼ������
void Load_Net_Parameters_Client(void);	//װ���������(�ͻ���ģʽ)
void Load_Net_Parameters_Server(void);	//װ���������(�����ģʽ)
void Load_Net_Parameters_UDP(void);			//װ���������(UDPģʽ)
void W5500_Socket_Set(void);						//W5500�˿ڳ�ʼ������
void Process_Socket_Data(SOCKET s);			//W5500���ղ����ͽ��յ�������

#endif
