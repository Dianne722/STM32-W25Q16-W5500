#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "httpclient.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#define FLASH_START_ADDR  0x8006000  // д���ַ��ʼλ��
//#define FLASH_PAGE_SIZE   1024      // STM32F103C8 ÿҳ��СΪ 1KB
#define HTTP_HEADER_END "\r\n\r\n"
#define FLASH_END_ADDR  0x8007000

uint8_t flag;
uint32_t content_length = 0;  // �����ݳ���
uint32_t received_length = 0; // �ѽ������ݳ���

/**
 * @brief   ����HTTP GET�����
 * @param   pkt: �������������
 * @return  ���������
 */
uint32_t HTTP_GetPkt(uint8_t *pkt)
{
    *pkt = 0;
    strcat((char*)pkt, "GET /upload/684529866.bin HTTP/1.1\r\n");  // ����ver 3.0.0
    strcat((char*)pkt, "Host: www.ibinhub.com");
    strcat((char*)pkt, "\r\n\r\n");

    return strlen((char*)pkt);
}

/**
 * @brief   д���ݵ�Flash��
 * @param   data:     ����ָ��
 * @param   length:   ���ݳ���
 * @param   flash_address: д��� Flash ��ַ
 * @return  ���º�� Flash ��ַ
 */
uint32_t write_to_flash(uint32_t flash_address, uint8_t *data, uint32_t length)
{
    HAL_FLASH_Unlock();  // ���� Flash д��

    // ���� Flash ҳ
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = flash_address;
    erase_init.NbPages = (length + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE; // ȷ�������㹻���ҳ��

    // ��ҳ�������������ʧ��
    for (uint32_t page = 0; page < erase_init.NbPages; page++)
    {
        erase_init.PageAddress = flash_address + (page * FLASH_PAGE_SIZE);
        if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK)
        {
            printf("Flash erase failed. Error: %lu at address: 0x%08lx\n", page_error, erase_init.PageAddress);
            HAL_FLASH_Lock();
            return flash_address;  // ����ʱ����ԭ��ַ
        }
    }

    // д�� Flash ����
    for (uint32_t i = 0; i < length; i += 4)
    {
        uint32_t data_word = 0xFFFFFFFF; // Ĭ�����
        memcpy(&data_word, data + i, (length - i) >= 4 ? 4 : (length - i));

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_address, data_word) != HAL_OK)
        {
            printf("Flash write failed at address: 0x%08lx\n", flash_address);
            HAL_FLASH_Lock();
            return flash_address;  // ����ʱ����ԭ��ַ
        }

        // ��֤�����Ƿ���ȷд��
        uint32_t read_back = *(volatile uint32_t *)flash_address;
        if (read_back != data_word)
        {
            printf("Flash data verification failed at address: 0x%08lx. Expected: 0x%08lx, Got: 0x%08lx\n", 
                   flash_address, data_word, read_back);
            HAL_FLASH_Lock();
            return flash_address;  // ����ʱ����ԭ��ַ
        }

        flash_address += 4;
    }

    printf("Flash write completed.\n");
    HAL_FLASH_Lock();  // ���� Flash д��
    return flash_address;
}


/**
 * @brief   ��ȡHTTPͷ���е�Content-Length�ֶ�
 * @param   header: HTTPͷ��
 * @return  content_length: Content-Length��ֵ
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
 * @brief   HTTP Client��ȡ������
 * @param   sn:      socket���
 * @param   buf:     ��������
 * @param   destip:  Ŀ��IP
 * @param   destport:Ŀ��˿�
 * @return  �޷���ֵ
 */
void do_http_client(uint8_t sn, uint8_t *buf, uint8_t *destip, uint16_t destport)
{
    uint16_t local_port = 50000;
    uint16_t len;
    static uint32_t flash_address = FLASH_START_ADDR; // ��̬���µ�Flash��ַ

    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        connect(sn, destip, destport);
        break;

    case SOCK_ESTABLISHED:
        if (flag == 0) // ����HTTP GET����
        {
            len = HTTP_GetPkt(buf);
            send(sn, buf, len);
            flag = 1;
        }
        else if (flag == 1) // ����HTTP��Ӧ
        {
            len = getSn_RX_RSR(sn);
            if (len > 0)
            {
                len = recv(sn, buf, len);

                // ����ǵ�һ�ν��գ�����HTTPͷ
                if (content_length == 0)
                {
                    char *header_end = strstr((char *)buf, HTTP_HEADER_END);
                    if (header_end)
                    {
                        uint32_t header_size = header_end - (char *)buf + strlen(HTTP_HEADER_END);
                        content_length = get_content_length((char *)buf);
                        printf("Content-Length: %lu\n", content_length);

                        // д���һ�����ݿ�
                        flash_address = write_to_flash(flash_address, buf + header_size, len - header_size);
                        received_length += len - header_size;
                    }
                }
                else
                {
                    // ֱ��д��������ݿ�
                    flash_address = write_to_flash(flash_address, buf, len);
                    received_length += len;
                }

                // ��ӡ����
                printf("Received: %lu/%lu bytes\n", received_length, content_length);

                // ����Ƿ����
                if (received_length >= content_length)
                {
                    printf("File transfer complete.\n");
                    flag = 2; // ������
                }
            }
        }
        break;

    case SOCK_CLOSE_WAIT:
        close(sn);
        break;

    case SOCK_CLOSED:
        if (flag != 2) // ���δ��ɣ�����������
        {
            close(sn);
            socket(sn, Sn_MR_TCP, local_port, 0x00);
        }
        break;

    default:
        break;
    }
}
