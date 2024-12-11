#ifndef __LEAF_OTA_H_
#define __LEAF_OTA_H_


#define PageSize		FLASH_PAGE_SIZE			//1K

/*=====用户配置(根据自己的分区进行配置)=====*/
#define BootLoader_Size 		0x2000U			///< BootLoader的大小 8K
#define Application_Size		0x1000U			///< 应用程序的大小 4K

#define Application_1_Addr		0x08002000U		///< 应用程序1的首地址
#define Application_2_Addr		0x08003000U		///< 应用程序2的首地址
/*==========================================*/



/* 启动的步骤 */
#define Startup_Normol 0xFFFFFFFF	///< 正常启动
#define Startup_Update 0xAAAAAAAA	///< 升级再启动
#define Startup_Reset  0x5555AAAA	///< ***恢复出厂 目前没使用***



void Start_BootLoader(void);

#endif

