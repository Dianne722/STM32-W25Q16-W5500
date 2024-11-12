#ifndef __W5500_USER_H
#define __W5500_USER_H	
#include "W5500.h"

extern unsigned int W5500_Send_Delay_Counter;		//W5500发送延时计数变量(ms)

void RCC_Configuration(void);						//设置系统时钟为72MHZ(这个可以根据需要改)
void NVIC_Configuration(void);					//STM32中断向量表配配置
void Timer2_Init_Config(void);					//Timer2初始化配置
void System_Initialization(void);				//STM32系统初始化函数(初始化STM32时钟及外设)
void Delay(unsigned int d);							//延时函数(ms)

void W5500_Initialization(void);				//W5500初始化配置
void Load_Net_Parameters_Client(void);	//装载网络参数(客户端模式)
void Load_Net_Parameters_Server(void);	//装载网络参数(服务端模式)
void Load_Net_Parameters_UDP(void);			//装载网络参数(UDP模式)
void W5500_Socket_Set(void);						//W5500端口初始化配置
void Process_Socket_Data(SOCKET s);			//W5500接收并发送接收到的数据

#endif
