#include "user_main.h"
#include <stdio.h>
#include <stdint.h>
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "do_dns.h"
#include "httpclient.h"

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
    .ip = {192, 168, 1, 212},
    .gw = {192, 168, 1, 1},
    .sn = {255, 255, 255, 0},
    .dns = {8, 8, 8, 8},
    .dhcp = NETINFO_DHCP  // 使用 DHCP 获取 IP
    //.dhcp = NETINFO_STATIC  // 使用静态 IP
};

uint8_t org_server_name[] = "www.ibinhub.com";
uint8_t org_server_ip[4] = {0}; /* www.ibinhub.com 的 IP 地址将由 DNS 获取 */
uint8_t org_port = 80;           /* www.ibinhub.com 使用的端口 */
static uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

/**
 * @brief   User Run Program
 * @param   none
 * @return  none
 */
void user_run(void)
{
    printf("wizchip HTTP Client example\r\n");

    /* 初始化 wizchip */
    wizchip_initialize();

    /* 设置网络信息 */
    network_init(ethernet_buf, &default_net_info);

    /* DNS 解析服务器 IP 地址 */
    if (do_dns(ethernet_buf, org_server_name, org_server_ip))
    {
        if (org_server_ip[0] != 0)
        {
            printf("Server IP Address: %d.%d.%d.%d\r\n", org_server_ip[0], org_server_ip[1], org_server_ip[2], org_server_ip[3]);
        }
        else
        {
            printf("Unable to resolve server IP address.\r\n");
            while(1) {} // 停留在此处，等待进一步的处理
        }
    }

    /* 进入 HTTP 下载循环 */
    while (1)
    {
        // 调用 do_http_client 来下载并通过 printf 打印文件内容
        do_http_client(SOCKET_ID, ethernet_buf, org_server_ip, org_port);
    }
}
