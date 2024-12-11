#ifndef __HTTPCLIENT_H_
#define __HTTPCLIENT_H_

uint32_t HTTP_GetPkt(uint8_t *pkt);
void do_http_client(uint8_t sn, uint8_t *buf, uint8_t *destip, uint16_t destport);

#endif
