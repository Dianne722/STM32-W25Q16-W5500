#ifndef __LEAF_OTA_H_
#define __LEAF_OTA_H_


#define PageSize		FLASH_PAGE_SIZE			//1K

/*=====�û�����(�����Լ��ķ�����������)=====*/
#define BootLoader_Size 		0x2000U			///< BootLoader�Ĵ�С 8K
#define Application_Size		0x1000U			///< Ӧ�ó���Ĵ�С 4K

#define Application_1_Addr		0x08002000U		///< Ӧ�ó���1���׵�ַ
#define Application_2_Addr		0x08003000U		///< Ӧ�ó���2���׵�ַ
/*==========================================*/



/* �����Ĳ��� */
#define Startup_Normol 0xFFFFFFFF	///< ��������
#define Startup_Update 0xAAAAAAAA	///< ����������
#define Startup_Reset  0x5555AAAA	///< ***�ָ����� Ŀǰûʹ��***



void Start_BootLoader(void);

#endif

