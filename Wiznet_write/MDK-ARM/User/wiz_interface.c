#include "wiz_interface.h"
#include "wiz_platform.h"
#include "wizchip_conf.h"
#include "dhcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W5500_VERSION 0x04

struct wiz_timer
{
    void (*func)(void);     // Callback function pointer for a function to execute when the timer fires
    uint32_t trigger_time;  // Trigger time(ms)
    uint32_t count_time;    // Current count value(ms)
    struct wiz_timer *next; // next timer node
};

struct wiz_timer *wiz_timer_head = NULL;
volatile uint32_t wiz_delay_ms_count = 0;

/**
 * @brief Create wiz_timer Node
 * @param func :Callback function pointer for a function to execute when the timer fires
 * @param time :Trigger time, usually in milliseconds
 *
 * @return Returns pointer to wiz_timer node when successfully created, otherwise returns NULL
 */
static struct wiz_timer *create_wiz_timer_node(void (*func)(void), uint32_t time)
{
    struct wiz_timer *newNode = (struct wiz_timer *)malloc(sizeof(struct wiz_timer));
    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    newNode->func = func;
    newNode->trigger_time = time;
    newNode->count_time = 0;
    newNode->next = NULL;
    return newNode;
}

/**
 * @brief Add a new timer node to the timer chain table
 * @param func Callback function pointer, called when the timer time is reached
 * @param time Trigger time (ms)
 */
void wiz_add_timer(void (*func)(void), uint32_t time)
{
    struct wiz_timer *newNode = create_wiz_timer_node(func, time);
    if (wiz_timer_head == NULL)
    {
        wiz_timer_head = newNode;
        return;
    }
    struct wiz_timer *temp = wiz_timer_head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = newNode;
}

/**
 * @brief Delete the timer for the specified callback function
 * @param func callback function pointer
 * @return none
 */
void wiz_delete_timer(void (*func)(void))
{
    struct wiz_timer *temp = wiz_timer_head;
    struct wiz_timer *prev = NULL;

    if (temp != NULL && temp->func == func)
    {
        wiz_timer_head = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp->func != func)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return;

    prev->next = temp->next;
    free(temp);
}

/**
 * @brief wiz timer event handler
 *
 * You must add this function to your 1ms timer interrupt
 *
 */
void wiz_timer_handler(void)
{

    wiz_delay_ms_count++;
    struct wiz_timer *temp = wiz_timer_head;
    while (temp != NULL)
    {
        temp->count_time++;
        if (temp->count_time >= temp->trigger_time)
        {
            temp->count_time = 0;
            temp->func();
        }
        temp = temp->next;
    }
}

/**
 * @brief Delay function in milliseconds
 * @param nms :Delay Time
 */
void wiz_user_delay_ms(uint32_t nms)
{
    wiz_delay_ms_count = 0;
    while (wiz_delay_ms_count < nms)
    {
    }
}

/**
 * @brief Check the WIZCHIP version
 */
void wizchip_version_check(void)
{
    uint8_t error_count = 0;
    while (1)
    {
        wiz_user_delay_ms(1000);
        if (getVERSIONR() != W5500_VERSION)
        {
            error_count++;
            if (error_count > 5)
            {
                printf("error, W5500 version is 0x%02x, but read W5500 version value = 0x%02x\r\n", W5500_VERSION, getVERSIONR());
                while (1)
                    ;
            }
        }
        else
        {
            break;
        }
    }
}

/**
 * @brief Print PHY information
 */
void wiz_print_phy_info(void)
{
    uint8_t get_phy_conf;
    get_phy_conf = getPHYCFGR();
    printf("The current Mbtis speed : %dMbps\r\n", get_phy_conf & 0x02 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_phy_conf & 0x04 ? "Full-Duplex" : "Half-Duplex");
}

/**
 * @brief Ethernet Link Detection
 */
void wiz_phy_link_check(void)
{
    uint8_t phy_link_status;
    do
    {
        wiz_user_delay_ms(1000);
        ctlwizchip(CW_GET_PHYLINK, (void *)&phy_link_status);
        if (phy_link_status == PHY_LINK_ON)
        {
            printf("PHY link\r\n");
            wiz_print_phy_info();
        }
        else
        {
            printf("PHY no link\r\n");
        }
    } while (phy_link_status == PHY_LINK_OFF);
}

/**
 * @brief   wizchip init function
 * @param   none
 * @return  none
 */
void wizchip_initialize(void)
{
    /* Enable timer interrupt */
    wiz_tim_irq_enable();

    /* reg wizchip spi */
    wizchip_spi_cb_reg();

    /* Reset the wizchip */
    wizchip_reset();

    /* Read version register */
    wizchip_version_check();

    /* Check PHY link status, causes PHY to start normally */
    wiz_phy_link_check();
}

/**
 * @brief   print network information
 * @param   none
 * @return  none
 */
void print_network_information(void)
{
    wiz_NetInfo net_info;
    wizchip_getnetinfo(&net_info); // Get chip configuration information

    if (net_info.dhcp == NETINFO_DHCP)
    {
        printf("====================================================================================================\r\n");
        printf(" %s network configuration : DHCP\r\n\r\n", _WIZCHIP_ID_);
    }
    else
    {
        printf("====================================================================================================\r\n");
        printf(" %s network configuration : static\r\n\r\n", _WIZCHIP_ID_);
    }

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\r\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("====================================================================================================\r\n\r\n");
}

/**
 * @brief DHCP process
 * @param sn :socket number
 * @param buffer :socket buffer
 */
static uint8_t wiz_dhcp_process(uint8_t sn, uint8_t *buffer)
{
    wiz_NetInfo conf_info;
    uint8_t dhcp_run_flag = 1;
    uint8_t dhcp_ok_flag = 0;
    /* Registration DHCP_time_handler to 1 second timer */
    wiz_add_timer(DHCP_time_handler, 1000);
    DHCP_init(sn, buffer);
    printf("DHCP running\r\n");
    while (1)
    {
        switch (DHCP_run()) // Do the DHCP client
        {
        case DHCP_IP_LEASED: // DHCP Acquiring network information successfully
        {
            if (dhcp_ok_flag == 0)
            {
                dhcp_ok_flag = 1;
                dhcp_run_flag = 0;
            }
            break;
        }
        case DHCP_FAILED:
        {
            dhcp_run_flag = 0;
            break;
        }
        }
        if (dhcp_run_flag == 0)
        {
            printf("DHCP %s!\r\n", dhcp_ok_flag ? "success" : "fail");
            DHCP_stop();

            /*DHCP obtained successfully, cancel the registration DHCP_time_handler*/
            wiz_delete_timer(DHCP_time_handler);

            if (dhcp_ok_flag)
            {
                getIPfromDHCP(conf_info.ip);
                getGWfromDHCP(conf_info.gw);
                getSNfromDHCP(conf_info.sn);
                getDNSfromDHCP(conf_info.dns);
                conf_info.dhcp = NETINFO_DHCP;
                getSHAR(conf_info.mac);
                wizchip_setnetinfo(&conf_info); // Update network information to network information obtained by DHCP
                return 1;
            }
            return 0;
        }
    }
}

/**
 * @brief   set network information
 *
 * First determine whether to use DHCP. If DHCP is used, first obtain the Internet Protocol Address through DHCP.
 * When DHCP fails, use static IP to configure network information. If static IP is used, configure network information directly
 *
 * @param   sn: socketid
 * @param   ethernet_buff:
 * @param   net_info: network information struct
 * @return  none
 */
void network_init(uint8_t *ethernet_buff, wiz_NetInfo *conf_info)
{
    int ret;
    wizchip_setnetinfo(conf_info); // Configuring Network Information
    if (conf_info->dhcp == NETINFO_DHCP)
    {
        ret = wiz_dhcp_process(0, ethernet_buff);
        if (ret == 0)
        {
            conf_info->dhcp = NETINFO_STATIC;
            wizchip_setnetinfo(conf_info);
        }
    }
    print_network_information();
}
