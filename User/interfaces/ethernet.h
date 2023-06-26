#ifndef USER_INTERFACES_ETHERNET_H_
#define USER_INTERFACES_ETHERNET_H_

#include "ch32v30x.h"

#include "eth_driver.h"

#define POS_FRAME_TYPE_HW 12
#define POS_FRAME_TYPE_LW 13

#define FRAME_TYPE_UDP ((uint16_t)0x0800)
#define FRAME_TYPE_ARP ((uint16_t)0x0806)

#define ARP_FRAME_SIZE 42
#define ARP_OPCODE_REQUEST 1
#define ARP_OPCODE_REPLY 2
typedef struct
{
    uint8_t dstMAC[6];
    uint8_t srcMAC[6];

    uint16_t frameType;

    uint16_t hardwareType;
    uint16_t protocolType;
    uint8_t hardwareSize;
    uint8_t protocolSize;

    uint16_t opCode;

    uint8_t senderHwAdr[6];
    uint8_t senderIpAdr[4];

    uint8_t targetHwAdr[6];
    uint8_t targetIpAdr[4];
}ARPFrame_str;

typedef union
{
    ARPFrame_str structData;
    uint8_t      rawData[ARP_FRAME_SIZE];
}ARPFrame;

void ETHERNET_ParseUdpFrame(const RecievedFrameData* frame);
void ETHERNET_ParseIcmpFrame(const RecievedFrameData* frame);
void ETHERNET_ParseArpFrame(const RecievedFrameData* frame);

void ETHERNET_Init();
void ETHERNET_HandleGlobalInt();

#endif /* USER_INTERFACES_ETHERNET_H_ */
