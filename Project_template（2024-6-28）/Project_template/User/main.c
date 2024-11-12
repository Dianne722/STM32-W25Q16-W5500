/***************************************************************************************
 * 工程名      ：客户端/服务端/UDP模式例程(客户端模式、服务端模式与UDP模式的区别在于：装载网络参数不同)
 * 描述        ：1、W5500的端口0工作在客户端模式，与《TCP&UDP测试工具》上创建的服务端连接,
 *                 并且以500ms的时间间隔定时给服务端发送字符串"\r\n你好，优信电子！！！\r\n"。
 *                 同时将接收到服务端发来的数据回发给服务端。
 *              2、W5500的端口0工作在服务端模式,与《TCP&UDP测试工具》上创建的客户端主动与服务端连接,
 *                 连接成功后，服务端以500ms的时间间隔定时给客户端发送字符串"\r\n你好，优信电子！！！\r\n"。
 *                 同时将接收到客户端发来的数据回发给客户端。
 * 实验平台   ：STM32F103C8T6开发板 + W5500以太网(TCP/IP)模块 + 上位机（电脑）
 * 硬件连接   ：PC15 -> W5500_RST
 *              PA4 -> W5500_SCS
 *              PA5 -> W5500_SCK
 *              PA6 -> W5500_MISO
 *              PA7 -> W5500_MOSI
***************************************************************************************/
#include "W5500.h"
#include "W5500_USER.h"
#include <string.h>


int main(void)
{
	System_Initialization();											//STM32系统初始化函数(初始化STM32时钟及外设)

	//Load_Net_Parameters_Client();									//装载网络参数（客户端模式）
	Load_Net_Parameters_Server();									//装载网络参数（服务端模式）
	
//	Load_Net_Parameters_UDP();										//装载网络参数(UDP模式)
	
	W5500_Hardware_Reset();											//硬件复位W5500
	W5500_Initialization();											//W5500初始化配置
	while (1)
	{
		W5500_Socket_Set();											//W5500端口初始化配置

		W5500_Interrupt_Process();									//W5500中断处理程序框架
		
//		if(W5500_Interrupt)											//处理W5500中断
//		{
//			W5500_Interrupt_Process();								//W5500中断处理程序框架
//		}

		if((S0_Data & S_RECEIVE) == S_RECEIVE)			            //如果Socket0接收到数据
		{
			S0_Data &= ~S_RECEIVE;
			Process_Socket_Data(0);									//W5500接收并发送接收到的数据
		}
		else if(W5500_Send_Delay_Counter >= 500)		            //定时发送字符串
		{
			if(S0_State == (S_INIT|S_CONN))
			{
				S0_Data &= ~S_TRANSMITOK;
				memcpy(Tx_Buffer, "\r\n你好，小助科技！！！\r\n", 24);
				Write_SOCK_Data_Buffer(0, Tx_Buffer, 24);			//指定Socket(0~7)发送数据处理,端口0发送23字节数据
			}
			W5500_Send_Delay_Counter=0;
		}
	}
}
