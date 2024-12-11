#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H
 
#include "main.h"
#include "gpio.h"
#define FLASH_BASE_ADDR    (uint32_t)(0x08000000)
 
//BootLoader   预留10KB的FLASH空间 --- (0x0800 0000 --> 0x0800 27FF)
//Application1 预留20KB的FLASH空间 --- (0x0800 2800 --> 0x0800 77FF)
//Application2 预留20KB的FLASH空间 --- (0x0800 7800 --> 0x0800 C7FF)
 
#define BOOT_BASE_ADDR    FLASH_BASE_ADDR
#define APP1_BASE_ADDR    (uint32_t)(0x08002800)
#define APP2_BASE_ADDR    (uint32_t)(0x08007800)
 
void BootLoader_Code(void);

#endif

