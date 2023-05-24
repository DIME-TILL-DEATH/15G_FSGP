#ifndef USER_INTERFACES_ETHERNET_H_
#define USER_INTERFACES_ETHERNET_H_

#include "ch32v30x.h"

#include "wchnet.h"
#include "eth_driver.h"

typedef void (*parser_ptr)(uint8_t*, uint32_t, uint8_t*, uint32_t*);

void ETHERNET_Init(parser_ptr func);
void WCHNET_HandleGlobalInt();

void mStopIfError(u8 iError);
void WCHNET_CreateUdpSocket(void);

#endif /* USER_INTERFACES_ETHERNET_H_ */
