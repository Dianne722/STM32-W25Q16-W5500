#include "user_main.h"
#include <stdio.h>
#include <stdint.h>
#include "wiz_interface.h"

/*wizchip->STM32 Hardware Pin define*/
//	wizchip_SCS    --->     STM32_GPIOA4
//	wizchip_SCLK	 --->     STM32_GPIOA5
//  wizchip_MISO	 --->     STM32_GPIOA6
//	wizchip_MOSI	 --->     STM32_GPIOA7
//	wizchip_RESET	 --->     STM32_GPIOC15
//	wizchip_INT    --->     STM32_GPIOC14

/* network information */
wiz_NetInfo default_net_info = {
    .mac = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12},
    .ip = {192, 168, 1, 110},
    .gw = {192, 168, 1, 1},
    .sn = {255, 255, 255, 0},
    .dns = {8, 8, 8, 8},
    .dhcp = NETINFO_DHCP}; // DHCP get ip address
uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
/**
 * @brief   User Run Program
 * @param   none
 * @return  none
 */
void user_run(void)
{
	wiz_NetInfo net_info;
  printf("wizchip dhcp example\r\n");

  /* wizchip init */
  wizchip_initialize();

  /* First use DHCP to obtain the Internet Protocol Address, and use a static address if the maximum number of reconnections is exceeded */
  network_init(ethernet_buf, &default_net_info);
	
	wizchip_getnetinfo(&net_info);
  printf("please try ping %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);

  while (1)
  {
	}
}
