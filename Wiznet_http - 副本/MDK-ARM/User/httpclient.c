#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "httpclient.h"

uint8_t flag;

/**
 * @brief   HTTP GET :  Request package combination package.
 * @param   pkt:    Array cache for grouping packages
 * @return  pkt:    Package length
 */
uint32_t HTTP_GetPkt(uint8_t *pkt)
{
    *pkt = 0;
    strcat((char*)pkt, "GET /upload/771004961.bin HTTP/1.1\r\n");
    strcat((char*)pkt, "Host: www.ibinhub.com");
    strcat((char*)pkt, "\r\n\r\n");

    return strlen((char*)pkt);
}

/**
 * @brief   HTTP Client file download and print via printf.
 * @param   sn:         socket number
 * @param   buf:        array cache
 * @param   destip:     destination IP
 * @param   destport:   destination port
 * @return  none
 */
void do_http_client(uint8_t sn, uint8_t *buf, uint8_t *destip, uint16_t destport)
{
    uint16_t local_port = 50000;
    uint16_t len;

    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        connect(sn, destip, destport);
        break;
    case SOCK_ESTABLISHED:
        if (flag == 0)
        {
            len = HTTP_GetPkt(buf);
            send(sn, buf, len);
            flag = 1;
        }
        else
        {
            len = getSn_RX_RSR(sn);  // Check if there is data to read
            if (len > 0)
            {
                len = recv(sn, buf, len);
                for (uint16_t i = 0; i < len; i++)
                {
                    printf("%c", *(buf + i));  // 使用 printf 输出到串口
                }
            }
        }
        break;
    case SOCK_CLOSE_WAIT:
        close(sn);
        break;
    case SOCK_CLOSED:
        close(sn);
        socket(sn, Sn_MR_TCP, local_port, 0x00);
        flag = 0;  // Reset flag for next request
        break;
    default:
        break;
    }
}
