#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_
 
#include "main.h"
 
 
#define  APP_ADDR  0x08006000   //应用程序首地址定义 
 
/*选择性开启相应的LOG信息*/
#define BOOTLOADER_LOG		
 
#define CLOSE_ALL_INT()  __set_PRIMASK(1)	//关闭所有中断
typedef void (*APP_FUNC)(); 				//函数指针类型定义
 
void iap_load_app(void);	//跳转函数
 
#endif
 
 