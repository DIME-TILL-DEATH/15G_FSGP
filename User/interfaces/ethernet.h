#ifndef USER_INTERFACES_ETHERNET_H_
#define USER_INTERFACES_ETHERNET_H_

#include "ch32v30x.h"

#include "eth_driver.h"

#define ETHERNETII_HEADER_SIZE 14

#define IP_HEADER_SIZE 20

#define POS_FRAME_TYPE_HW 12
#define POS_FRAME_TYPE_LW 13
#define POS_PROTOCOL 23

#define FRAME_TYPE_IPv4 ((uint16_t)0x0800)
#define FRAME_TYPE_ARP ((uint16_t)0x0806)

#define IPv4_PROTOCOL_ICMP 1
#define IPv4_PROTOCOL_UDP 17

#define ARP_FULL_HEADER_SIZE 42
#define ARP_OPCODE_REQUEST 1
#define ARP_OPCODE_REPLY 2
typedef struct
{
    // ETHERNETII
    uint8_t dstMAC[6];
    uint8_t srcMAC[6];

    uint16_t frameType;

    // ARP
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
    uint8_t      rawData[ARP_FULL_HEADER_SIZE];
}ARPFrame;

#define UDP_FULL_HEADER_SIZE 42
#define UDP_ONLY_HEADER_SIZE 8
#define UDP_PSEUDO_HEADER_SIZE 12
#define UDP_PAYLOAD_POSITION UDP_FULL_HEADER_SIZE
typedef struct
{
    // ETHERNETII
    uint8_t dstMAC[6];
    uint8_t srcMAC[6];

    uint16_t frameType;

    // IP
    uint8_t ipVerHdrLen;
    uint8_t diffServicesField;

    uint16_t ipTotalLength;
    uint16_t identification;

    uint16_t fragmentFlagsAndOffset;

    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;

    uint8_t srcIpAddress[4];
    uint8_t dstIpAddress[4];

    // UDP
    uint16_t srcPort;
    uint16_t dstPort;

    uint16_t udpLength;
    uint16_t udpCheckSum;

}UDPFrame_str;

typedef union
{
    UDPFrame_str structData;
    uint8_t rawData[UDP_FULL_HEADER_SIZE];
}UDPFrame;

#define ICMP_FULL_HEADER_SIZE 74

#define ICMP_TYPE_ECHO_REQUEST 8
#define ICMP_TYPE_ECHO_ANSWER 0
typedef struct
{
    // ETHERNETII
    uint8_t dstMAC[6];
    uint8_t srcMAC[6];

    uint16_t frameType;

    // IP
    uint8_t ipVerHdrLen;
    uint8_t diffServicesField;

    uint16_t ipTotalLength;
    uint16_t identification;

    uint16_t fragmentFlagsAndOffset;

    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;

    uint8_t srcIpAddress[4];
    uint8_t dstIpAddress[4];

    // ICMP
    uint8_t icmpType;
    uint8_t icmpCode;
    uint16_t icmpChecksum;

    uint16_t icmpId;
    uint16_t icmpSeqNum;

    uint8_t icmpData[32];
}ICMPFrame_str;

typedef union
{
    ICMPFrame_str structData;
    uint8_t rawData[ICMP_FULL_HEADER_SIZE];
}ICMPFrame;

void ETHERNET_ParseUdpFrame(const RecievedFrameData* frame);
void ETHERNET_ParseIcmpFrame(const RecievedFrameData* frame);
void ETHERNET_ParseArpFrame(const RecievedFrameData* frame);

void ETHERNET_Init();
void ETHERNET_HandleGlobalInt();

#endif /* USER_INTERFACES_ETHERNET_H_ */
