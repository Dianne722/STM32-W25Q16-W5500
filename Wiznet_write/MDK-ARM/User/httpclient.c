#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "httpclient.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#define FLASH_START_ADDR  0x8006000  // 写入地址起始位置
//#define FLASH_PAGE_SIZE   1024      // STM32F103C8 每页大小为 1KB
#define HTTP_HEADER_END "\r\n\r\n"
#define FLASH_END_ADDR  0x8007000

uint8_t flag;
uint32_t content_length = 0;  // 总数据长度
uint32_t received_length = 0; // 已接收数据长度

/**
 * @brief   生成HTTP GET请求包
 * @param   pkt: 请求包缓存数组
 * @return  请求包长度
 */
uint32_t HTTP_GetPkt(uint8_t *pkt)
{
    *pkt = 0;
    strcat((char*)pkt, "GET /upload/684529866.bin HTTP/1.1\r\n");  // 下载ver 3.0.0
    strcat((char*)pkt, "Host: www.ibinhub.com");
    strcat((char*)pkt, "\r\n\r\n");

    return strlen((char*)pkt);
}

/**
 * @brief   写数据到Flash。
 * @param   data:     数据指针
 * @param   length:   数据长度
 * @param   flash_address: 写入的 Flash 地址
 * @return  更新后的 Flash 地址
 */
uint32_t write_to_flash(uint32_t flash_address, uint8_t *data, uint32_t length)
{
    HAL_FLASH_Unlock();  // 解锁 Flash 写入

    // 擦除 Flash 页
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = flash_address;
    erase_init.NbPages = (length + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE; // 确保擦除足够大的页面

    // 逐页擦除，避免擦除失败
    for (uint32_t page = 0; page < erase_init.NbPages; page++)
    {
        erase_init.PageAddress = flash_address + (page * FLASH_PAGE_SIZE);
        if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK)
        {
            printf("Flash erase failed. Error: %lu at address: 0x%08lx\n", page_error, erase_init.PageAddress);
            HAL_FLASH_Lock();
            return flash_address;  // 错误时返回原地址
        }
    }

    // 写入 Flash 数据
    for (uint32_t i = 0; i < length; i += 4)
    {
        uint32_t data_word = 0xFFFFFFFF; // 默认填充
        memcpy(&data_word, data + i, (length - i) >= 4 ? 4 : (length - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_address, data_word) != HAL_OK)
        {
            printf("Flash write failed at address: 0x%08lx\n", flash_address);
            HAL_FLASH_Lock();
            return flash_address;  // 错误时返回原地址
        }

        // 验证数据是否正确写入
        uint32_t read_back = *(volatile uint32_t *)flash_address;
        if (read_back != data_word)
        {
            printf("Flash data verification failed at address: 0x%08lx. Expected: 0x%08lx, Got: 0x%08lx\n", 
                   flash_address, data_word, read_back);
            HAL_FLASH_Lock();
            return flash_address;  // 错误时返回原地址
        }

        flash_address += 4;
    }

    printf("Flash write completed.\n");
    HAL_FLASH_Lock();  // 锁定 Flash 写入
    return flash_address;
}


/**
 * @brief   获取HTTP头部中的Content-Length字段
 * @param   header: HTTP头部
 * @return  content_length: Content-Length的值
 */
uint32_t get_content_length(const char *header)
{
    const char *cl_str = strstr(header, "Content-Length: ");
    if (cl_str)
    {
        return (uint32_t)atoi(cl_str + strlen("Content-Length: "));
    }
    return 0;
}


/**
 * @brief   HTTP Client获取数据流
 * @param   sn:      socket编号
 * @param   buf:     缓存数组
 * @param   destip:  目标IP
 * @param   destport:目标端口
 * @return  无返回值
 */
void do_http_client(uint8_t sn, uint8_t *buf, uint8_t *destip, uint16_t destport)
{
    uint16_t local_port = 50000;
    uint16_t len;
    static uint32_t flash_address = FLASH_START_ADDR; // 动态更新的Flash地址

    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        connect(sn, destip, destport);
        break;

    case SOCK_ESTABLISHED:
        if (flag == 0) // 发送HTTP GET请求
        {
            len = HTTP_GetPkt(buf);
            send(sn, buf, len);
            flag = 1;
        }
        else if (flag == 1) // 接收HTTP响应
        {
            len = getSn_RX_RSR(sn);
            if (len > 0)
            {
                len = recv(sn, buf, len);

                // 如果是第一次接收，解析HTTP头
                if (content_length == 0)
                {
                    char *header_end = strstr((char *)buf, HTTP_HEADER_END);
                    if (header_end)
                    {
                        uint32_t header_size = header_end - (char *)buf + strlen(HTTP_HEADER_END);
                        content_length = get_content_length((char *)buf);
                        printf("Content-Length: %lu\n", content_length);

                        // 写入第一个数据块
                        flash_address = write_to_flash(flash_address, buf + header_size, len - header_size);
                        received_length += len - header_size;
                    }
                }
                else
                {
                    // 直接写入后续数据块
                    flash_address = write_to_flash(flash_address, buf, len);
                    received_length += len;
                }

                // 打印进度
                printf("Received: %lu/%lu bytes\n", received_length, content_length);

                // 检查是否完成
                if (received_length >= content_length)
                {
                    printf("File transfer complete.\n");
                    flag = 2; // 标记完成
                }
            }
        }
        break;

    case SOCK_CLOSE_WAIT:
        close(sn);
        break;

    case SOCK_CLOSED:
        if (flag != 2) // 如果未完成，则重新连接
        {
            close(sn);
            socket(sn, Sn_MR_TCP, local_port, 0x00);
        }
        break;

    default:
        break;
    }
}
