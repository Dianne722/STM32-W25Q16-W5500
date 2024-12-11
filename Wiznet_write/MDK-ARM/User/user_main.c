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
    .dhcp = NETINFO_DHCP  // ʹ�� DHCP ��ȡ IP
    //.dhcp = NETINFO_STATIC  // ʹ�þ�̬ IP
};

uint8_t org_server_name[] = "www.ibinhub.com";
uint8_t org_server_ip[4] = {0}; /* www.ibinhub.com �� IP ��ַ���� DNS ��ȡ */
uint8_t org_port = 80;           /* www.ibinhub.com ʹ�õĶ˿� */
static uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

/**
 * @brief   User Run Program
 * @param   none
 * @return  none
 */
void user_run(void)
{
    printf("wizchip HTTP Client example\r\n");

    /* ��ʼ�� wizchip */
    wizchip_initialize();

    /* ����������Ϣ */
    network_init(ethernet_buf, &default_net_info);

    /* DNS ���������� IP ��ַ */
    if (do_dns(ethernet_buf, org_server_name, org_server_ip))
    {
        if (org_server_ip[0] != 0)
        {
            printf("Server IP Address: %d.%d.%d.%d\r\n", org_server_ip[0], org_server_ip[1], org_server_ip[2], org_server_ip[3]);
        }
        else
        {
            printf("Unable to resolve server IP address.\r\n");
            while(1) {} // ͣ���ڴ˴����ȴ���һ���Ĵ���
        }
    }

    /* ���� HTTP ����ѭ�� */
    while (1)
    {
        // ���� do_http_client �����ز�ͨ�� printf ��ӡ�ļ�����
        do_http_client(SOCKET_ID, ethernet_buf, org_server_ip, org_port);
    }
}
