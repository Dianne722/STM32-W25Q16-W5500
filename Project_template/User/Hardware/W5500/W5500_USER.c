/**********************************************************************************
 * �ļ���  ��W5500_USER.c
 * ����    ��W5500 ������
**********************************************************************************/
#include "stm32f10x.h"
#include "W5500.h"
#include <string.h>

unsigned int Timer2_Counter = 0; 								//Timer2��ʱ����������(ms)
unsigned int W5500_Send_Delay_Counter = 0;			//W5500������ʱ��������(ms)

/*******************************************************************************
* ������  :	RCC_Configuration
* ����    :	����ϵͳʱ��Ϊ72MHZ(������Ը�����Ҫ��)
* ����    :	��
* ���    :	��
* ����ֵ  :	��
* ˵��    :	STM32F107x��STM32F105xϵ��MCU��STM32F103xϵ��MCUʱ������������ͬ
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;               //�ⲿ����ʱ��(HSE)�Ĺ���״̬����
  
  RCC_DeInit();                               //����RCC���ã���������ʱ����صļĴ�������ΪĬ��ֵ
  RCC_HSEConfig(RCC_HSE_ON);                  //�����ⲿ����ʱ��HSE 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //�ȴ��ⲿ����ʱ��(HSE)�ȶ�

  if(SUCCESS == HSEStartUpStatus)             //����ⲿ����ʱ���Ѿ��ȶ�
  {
    /* Flash�洢������ */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //����FlashԤȡ����
    FLASH_SetLatency(FLASH_Latency_2);										//����Flash�ӳ�״̬Ϊ2������Ϊ����Ӧ72MHz�ĸ�Ƶʱ��
    
		/* ��������ʱ�ӷ�Ƶ */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 					//����AHBʱ�ӵ���ϵͳʱ��(1��Ƶ)/72MHZ
    RCC_PCLK2Config(RCC_HCLK_Div1);  					//����APB2ʱ�Ӻ�HCLKʱ�����/72MHz(���Ϊ72MHz)
    RCC_PCLK1Config(RCC_HCLK_Div2);  					//����APB1ʱ����HCLKʱ�ӵ�2��Ƶ/36MHz(���Ϊ36MHz)
		
		/* PLL���� */
#ifndef STM32F10X_CL                 											//���ʹ�õĲ���STM32F10X_CLϵ�У���STM32F103xϵ�У�MCU,PLL��������  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 	//����PLL������ΪHSE������Ƶ��PLL��Ƶ����Ϊ9����PLLCLK = 8MHz * 9 = 72 MHz 
#else                                											//���ʹ�õ���STM32F10X_CLϵ��(��STM32F107x��STM32F105xϵ�У�MCU,PLL��������
    /***** ����PLLx *****/
    /* ����PLL2�������Ƶ��Ϊ5��PLL2��ƵΪ8: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    RCC_PLL2Cmd(ENABLE); 																					//ʹ��PLL2 
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);					//�ȴ�PLL2�ȶ�

    /* ����PLL1������ΪPLL2���������Ƶ5��������PLL1�ı�Ƶ����Ϊ9: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif

    RCC_PLLCmd(ENABLE); 																		//ʹ��PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);			//�ȴ�PLL�ȶ�

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);							//����ϵͳʱ�ӵ�ʱ��ԴΪPLL

    while(RCC_GetSYSCLKSource() != 0x08);										//���ϵͳ��ʱ��Դ�Ƿ���PLL
    RCC_ClockSecuritySystemCmd(ENABLE);											//����ʱ�Ӱ�ȫϵͳ����������HSE����ʱ�Զ��л����ڲ�����ʱ�ӣ�HSI��

		/* ʹ������ʱ�� */
  	/* ʹ��TIM2��ʱ��ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  	/* ʹ��GPIOA��GPIOB��GPIOC��GPIOD��AFIO�����ù������������ʱ�� */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
					| RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
					| RCC_APB2Periph_AFIO, ENABLE);    
  }
}

/*******************************************************************************
* ������  : NVIC_Configuration
* ����    : ����STM32�ж��������ָ���жϵ����ȼ�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ʼ���������ض��ж�Դ��TIM2�������ԣ����������ȼ���ʹ��״̬
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;												//����NVIC��ʼ���ṹ��,���ں����趨�����жϵ����ò���

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);								//�����ж��������λ��,NVIC_VectTab_FLASHָʾ������λ���ڲ�Flash����ʼλ�ã�0x0��ƫ�Ƶ�ַ��
																															//�����жϷ�����ڵ���ʼ�㣬ͨ���ڻ���STM32��׼�����Ŀ��ֻ������һ�Ρ�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);							//�����ж����ȼ�����Ϊ1������2λ��ռ���ȼ���2λ��Ӧ���ȼ�������5�ַ���ģʽ��
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;							//�����ж������ţ�ָ�����õ��Ƕ�ʱ��2���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//�����������ȼ��������жϵ���ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//������Ӧ���ȼ��������жϵ������ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//ʹ��NVIC
	NVIC_Init(&NVIC_InitStructure);															//��������NVIC_InitStructure�е����ò���Ӧ�õ�NVIC�У�����жϵ�����
}

/*******************************************************************************
* ������  : Timer2_Init_Config
* ����    : Timer2��ʼ������
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void Timer2_Init_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);					//ʹ��Timer2ʱ��
	
	TIM_TimeBaseStructure.TIM_Period = 9;													//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ(������10Ϊ1ms)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;										//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ(10KHz�ļ���Ƶ��)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//����ʱ�ӷָ�:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);								//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );										//ʹ��TIM2ָ�����ж�
	
	TIM_Cmd(TIM2, ENABLE);																				//ʹ��TIMx����
}

/*******************************************************************************
* ������  : TIM2_IRQHandler
* ����    : ��ʱ��2�ж϶Ϸ�����
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
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
* ������  : System_Initialization
* ����    : STM32ϵͳ��ʼ������(��ʼ��STM32ʱ�Ӽ�����)
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void System_Initialization(void)
{
	RCC_Configuration();					//����ϵͳʱ��Ϊ72MHZ(������Ը�����Ҫ��)
  NVIC_Configuration();					//STM32�ж�������������
	SPI_Configuration();					//W5500 SPI��ʼ������(STM32 SPI1)
	Timer2_Init_Config();					//Timer2��ʼ������
	W5500_GPIO_Configuration();		//W5500 GPIO��ʼ������	
}

/*******************************************************************************
* ������  : Delay
* ����    : ��ʱ����(ms)
* ����    : d:��ʱϵ������λΪ����
* ���    : ��
* ����    : �� 
* ˵��    : ��ʱ������Timer2��ʱ��������1����ļ�����ʵ�ֵ�
*******************************************************************************/
void Delay(unsigned int d)
{
	Timer2_Counter = 0; 
	while(Timer2_Counter < d);
}

/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();				//��ʼ��W5500�Ĵ�������
	Detect_Gateway();		//������ط����� 
	Socket_Init(0);			//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
}

/*******************************************************************************
* ������  : Load_Net_Parameters_Client
* ����    : װ������������ͻ���ģʽ��
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
/*�����������*/
//���أ�192.168.1.1
//����:	255.255.255.0
//�����ַ��0C 29 AB 7C 00 01
//����IP��ַ:192.168.1.199
//�˿�0�Ķ˿ںţ�5000
//�˿�0��Ŀ��IP��ַ��192.168.1.190
//�˿�0��Ŀ�Ķ˿ںţ�6000
void Load_Net_Parameters_Client(void)
{
	Gateway_IP[0] = 192;		//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//������������
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//���������ַ
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//���ر���IP��ַ
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = 0x88;

	S0_DIP[0] = 192;				//���ض˿�0��Ŀ��IP��ַ
	S0_DIP[1] = 168;
	S0_DIP[2] = 1;
	S0_DIP[3] = 190;
	
	S0_DPort[0] = 0x17;			//���ض˿�0��Ŀ�Ķ˿ں�6000
	S0_DPort[1] = 0x70;

	S0_Mode = TCP_CLIENT;		//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ
}

/*******************************************************************************
* ������  : Load_Net_Parameters_Server
* ����    : װ���������(�����ģʽ)
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
/*�����������*/
//���أ�192.168.1.1
//����:	255.255.255.0
//�����ַ��0C 29 AB 7C 00 01
//����IP��ַ:192.168.1.199
//�˿�0�Ķ˿ںţ�5000
void Load_Net_Parameters_Server(void)
{
	Gateway_IP[0] = 192;		//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//������������
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//���������ַ
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//���ر���IP��ַ
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = 0x88;

	S0_Mode = TCP_SERVER;		//���ض˿�0�Ĺ���ģʽ,TCP�����ģʽ
}

/*******************************************************************************
* ������  : Load_Net_Parameters_UDP
* ����    : װ���������(UDPģʽ)
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
/*�����������*/
//���أ�192.168.1.1
//����:	255.255.255.0
//�����ַ��0C 29 AB 7C 00 01
//����IP��ַ:192.168.1.199
//�˿�0�Ķ˿ںţ�5000
void Load_Net_Parameters_UDP(void)
{
	Gateway_IP[0] = 192;		//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 1;
	Gateway_IP[3] = 1;

	Sub_Mask[0] = 255;			//������������
	Sub_Mask[1] = 255;
	Sub_Mask[2] = 255;
	Sub_Mask[3] = 0;

	Phy_Addr[0] = 0x0c;			//���������ַ
	Phy_Addr[1] = 0x29;
	Phy_Addr[2] = 0xab;
	Phy_Addr[3] = 0x7c;
	Phy_Addr[4] = 0x00;
	Phy_Addr[5] = 0x01;

	IP_Addr[0] = 192;				//���ر���IP��ַ
	IP_Addr[1] = 168;
	IP_Addr[2] = 1;
	IP_Addr[3] = 199;

	S0_Port[0] = 0x13;			//���ض˿�0�Ķ˿ں�5000 
	S0_Port[1] = 0x88;
	
	UDP_DIPR[0] = 192;			//UDP(�㲥)ģʽ,Ŀ������IP��ַ
	UDP_DIPR[1] = 168;
	UDP_DIPR[2] = 1;
	UDP_DIPR[3] = 190;

	UDP_DPORT[0] = 0x17;		//UDP(�㲥)ģʽ,Ŀ�������˿ں�
	UDP_DPORT[1] = 0x70;

	S0_Mode = UDP_MODE;			//���ض˿�0�Ĺ���ģʽ,UDPģʽ
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State == 0)//�˿�0��ʼ������
	{
		if(S0_Mode == TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(0) == TRUE)
				S0_State = S_INIT;
			else
				S0_State = 0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(0) == TRUE)
				S0_State = S_INIT|S_CONN;
			else
				S0_State = 0;
		}
	}
}

/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д���
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size = Read_SOCK_Data_Buffer(s, Rx_Buffer);
	memcpy(Tx_Buffer, Rx_Buffer, size);			
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}



