#ifndef __WIZ_INTERFACE_H__
#define __WIZ_INTERFACE_H__

#include "wizchip_conf.h"

/**
 * @brief Add a new timer node to the timer chain table
 * @param func Callback function pointer, called when the timer time is reached
 * @param time Trigger time (ms)
 */
void wiz_add_timer(void (*func)(void), uint32_t time);

/**
 * @brief Delete the timer for the specified callback function
 * @param func callback function pointer
 * @return none
 */
void wiz_delete_timer(void (*func)(void));

/**
 * @brief wiz timer event handler
 *
 * You must add this function to your 1ms timer interrupt
 *
 */
void wiz_timer_handler(void);

/**
 * @brief Delay function in milliseconds
 * @param nms :Delay Time
 */
void wiz_user_delay_ms(uint32_t nms);

/**
 * @brief   wizchip init function
 * @param   none
 * @return  none
 */
void wizchip_initialize(void);

/**
 * @brief   print network information
 * @param   none
 * @return  none
 */
void print_network_information(void);

/**
 * @brief   set network information
 * @param   sn: socketid
 * @param   ethernet_buff:
 * @param   net_info: network information struct
 * @return  none
 */
void network_init(uint8_t *ethernet_buff, wiz_NetInfo *conf_info);

/**
 * @brief Check the WIZCHIP version
 */
void wizchip_version_check(void);

/**
 * @brief Ethernet Link Detection
 */
void wiz_phy_link_check(void);

/**
 * @brief Print PHY information
 */
void wiz_print_phy_info(void);
#endif
