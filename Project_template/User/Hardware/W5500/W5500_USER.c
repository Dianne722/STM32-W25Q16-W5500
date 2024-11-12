/**********************************************************************************
 * 文件名  ：W5500_USER.c
 * 描述    ：W5500 函数库
**********************************************************************************/
#include "stm32f10x.h"
#include "W5500.h"
#include <string.h>

unsigned int Timer2_Counter = 0; 								//Timer2定时器计数变量(ms)
unsigned int W5500_Send_Delay_Counter = 0;			//W5500发送延时计数变量(ms)

/*******************************************************************************
* 函数名  :	RCC_Configuration
* 描述    :	设置系统时钟为72MHZ(这个可以根据需要改)
* 输入    :	无
* 输出    :	无
* 返回值  :	无
* 说明    :	STM32F107x和STM32F105x系列MCU与STM32F103x系列MCU时钟配置有所不同
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;               //外部高速时钟(HSE)的工作状态变量
  
  RCC_DeInit();                               //重置RCC配置，将所有与时钟相关的寄存器设置为默认值
  RCC_HSEConfig(RCC_HSE_ON);                  //启动外部高速时钟HSE 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //等待外部高速时钟(HSE)稳定

  if(SUCCESS == HSEStartUpStatus)             //如果外部高速时钟已经稳定
  {
    /* Flash存储器设置 */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //启用Flash预取缓冲
    FLASH_SetLatency(FLASH_Latency_2);										//设置Flash延迟状态为2，这是为了适应72MHz的高频时钟
    
		/* 设置总线时钟分频 */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 					//设置AHB时钟等于系统时钟(1分频)/72MHZ
    RCC_PCLK2Config(RCC_HCLK_Div1);  					//设置APB2时钟和HCLK时钟相等/72MHz(最大为72MHz)
    RCC_PCLK1Config(RCC_HCLK_Div2);  					//设置APB1时钟是HCLK时钟的2分频/36MHz(最大为36MHz)
		
		/* PLL配置 */
#ifndef STM32F10X_CL                 											//如果使用的不是STM32F10X_CL系列（如STM32F103x系列）MCU,PLL以下配置  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 	//配置PLL的输入为HSE，不分频，PLL倍频因子为9，即PLLCLK = 8MHz * 9 = 72 MHz 
#else                                											//如果使用的是STM32F10X_CL系列(如STM32F107x或STM32F105x系列）MCU,PLL以下配置
    /***** 配置PLLx *****/
    /* 配置PLL2的输入分频器为5，PLL2倍频为8: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    RCC_PLL2Cmd(ENABLE); 																					//使能PLL2 
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);					//等待PLL2稳定

    /* 配置PLL1的输入为PLL2的输出，分频5，并设置PLL1的倍频因子为9: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif

    RCC_PLLCmd(ENABLE); 																		//使能PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);			//等待PLL稳定

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);							//设置系统时钟的时钟源为PLL

    while(RCC_GetSYSCLKSource() != 0x08);										//检查系统的时钟源是否是PLL
    RCC_ClockSecuritySystemCmd(ENABLE);											//启用时钟安全系统，有助于在HSE故障时自动切换到内部高速时钟（HSI）

		/* 使能外设时钟 */
  	/* 使能TIM2定时器时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  	/* 使能GPIOA、GPIOB、GPIOC、GPIOD和AFIO（复用功能输入输出）时钟 */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
					| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
					| RCC_APB2Periph_AFIO, ENABLE);    
  }
}

/*******************************************************************************
* 函数名  : NVIC_Configuration
* 描述    : 配置STM32中断向量表和指定中断的优先级设置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 初始化和配置特定中断源（TIM2）的属性，包括其优先级和使能状态
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;												//定义NVIC初始化结构体,用于后续设定具体中断的配置参数

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);								//设置中断向量表的位置,NVIC_VectTab_FLASH指示向量表位于内部Flash的起始位置，0x0是偏移地址。
																															//这是中断服务入口的起始点，通常在基于STM32标准库的项目中只需设置一次。
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);							//设置中断优先级分组为1，即有2位抢占优先级和2位响应优先级（共有5种分组模式）
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;							//设置中断向量号，指定配置的是定时器2的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//设置抢先优先级，设置中断的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//设置响应优先级，设置中断的子优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//使能NVIC
	NVIC_Init(&NVIC_InitStructure);															//将设置在NVIC_InitStructure中的配置参数应用到NVIC中，完成中断的配置
}

/*******************************************************************************
* 函数名  : Timer2_Init_Config
* 描述    : Timer2初始化配置
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void Timer2_Init_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);					//使能Timer2时钟
	
	TIM_TimeBaseStructure.TIM_Period = 9;													//设置在下一个更新事件装入活动的自动重装载寄存器周期的值(计数到10为1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;										//设置用来作为TIMx时钟频率除数的预分频值(10KHz的计数频率)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//设置时钟分割:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);								//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );										//使能TIM2指定的中断
	
	TIM_Cmd(TIM2, ENABLE);																				//使能TIMx外设
}

/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		Timer2_Counter++;
		W5500_Send_Delay_Counter++;
	}
}

/*******************************************************************************
* 函数名  : System_Initialization
* 描述    : STM32系统初始化函数(初始化STM32时钟及外设)
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void System_Initialization(void)
{
	RCC_Configuration();					//设置系统时钟为72MHZ(这个可以根据需要改)
  NVIC_Configuration();					//STM32中断向量表配配置
	SPI_Configuration();					//W5500 SPI初始化配置(STM32 SPI1)
	Timer2_Init_Config();					//Timer2初始化配置
	W5500_GPIO_Configuration();		//W5500 GPIO初始化配置	
}

/*******************************************************************************
* 函数名  : Delay
* 描述    : 延时函数(ms)
* 输入    : d:延时系数，单位为毫秒
* 输出    : 无
* 返回    : 无 
* 说明    : 延时是利用Timer2定时器产生的1毫秒的计数来实现的
*******************************************************************************/
void Delay(unsigned int d)
{
	Timer2_Counter = 0; 
	while(Timer2_Counter < d);
}

/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();				//初始化W5500寄存器函数
	Detect_Gateway();		//检查网关服务器 
	Socket_Init(0);			//指定Socket(0~7)初始化,初始化端口0
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters_Client
* 描述    : 装载网络参数（客户端模式）
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
/*例程网络参数*/
//网关：192.168.1.1
//掩码:	255.255.255.0
//物理地址：0C 29 AB 7C 00 01
//本机IP地址:192.168.1.199
//端口0的端口号：5000
//端口0的目的IP地址：192.168.1.190
//端口0的目的端口号：6000
void Load_Net_Parameters_Client(void)
{
	Gateway_IP[0] = 192;		//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//加载子网掩码
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//加载物理地址
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//加载本机IP地址
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//加载端口0的端口号5000 
	S0_Port[1] = 0x88;

	S0_DIP[0] = 192;				//加载端口0的目的IP地址
	S0_DIP[1] = 168;
	S0_DIP[2] = 1;
	S0_DIP[3] = 190;
	
	S0_DPort[0] = 0x17;			//加载端口0的目的端口号6000
	S0_DPort[1] = 0x70;

	S0_Mode = TCP_CLIENT;		//加载端口0的工作模式,TCP客户端模式
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters_Server
* 描述    : 装载网络参数(服务端模式)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
/*例程网络参数*/
//网关：192.168.1.1
//掩码:	255.255.255.0
//物理地址：0C 29 AB 7C 00 01
//本机IP地址:192.168.1.199
//端口0的端口号：5000
void Load_Net_Parameters_Server(void)
{
	Gateway_IP[0] = 192;		//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//加载子网掩码
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//加载物理地址
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//加载本机IP地址
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//加载端口0的端口号5000 
	S0_Port[1] = 0x88;

	S0_Mode = TCP_SERVER;		//加载端口0的工作模式,TCP服务端模式
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters_UDP
* 描述    : 装载网络参数(UDP模式)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
/*例程网络参数*/
//网关：192.168.1.1
//掩码:	255.255.255.0
//物理地址：0C 29 AB 7C 00 01
//本机IP地址:192.168.1.199
//端口0的端口号：5000
void Load_Net_Parameters_UDP(void)
{
	Gateway_IP[0] = 192;		//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//加载子网掩码
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//加载物理地址
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//加载本机IP地址
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//加载端口0的端口号5000 
	S0_Port[1] = 0x88;
	
	UDP_DIPR[0] = 192;			//UDP(广播)模式,目的主机IP地址
	UDP_DIPR[1] = 168;
	UDP_DIPR[2] = 1;
	UDP_DIPR[3] = 190;

	UDP_DPORT[0] = 0x17;		//UDP(广播)模式,目的主机端口号
	UDP_DPORT[1] = 0x70;

	S0_Mode = UDP_MODE;			//加载端口0的工作模式,UDP模式
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State == 0)//端口0初始化配置
	{
		if(S0_Mode == TCP_SERVER)//TCP服务器模式 
		{
			if(Socket_Listen(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		}
		else//UDP模式 
		{
			if(Socket_UDP(0) == TRUE)
				S0_State = S_INIT|S_CONN;
			else
				S0_State = 0;
		}
	}
}

/*******************************************************************************
* 函数名  : Process_Socket_Data
* 描述    : W5500接收并发送接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size = Read_SOCK_Data_Buffer(s, Rx_Buffer);
	memcpy(Tx_Buffer, Rx_Buffer, size);			
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}



