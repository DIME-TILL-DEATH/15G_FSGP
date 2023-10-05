#include <string.h>

#include "ethernet.h"

#include "veeprom.h"
#include "frame_parser.h"

#define UDP_REC_BUF_LEN                1472
uint8_t MACAddr[6];                                              //MAC address
//uint8_t IPAddr[4] = {192, 168, 104, 10};                         //IP address
uint8_t IPAddr[4] = {10, 0, 0, 121};                         //IP address

uint8_t mstMACAddr[6] = {0};
uint8_t mstIPAddr[4] = {0};

uint8_t GWIPAddr[4] = {192, 168, 104, 255};                        //Gateway IP address
uint8_t IPMask[4] = {255, 255, 255, 0};                        //subnet mask

uint16_t dstPort = 40003;                                         //destination ports
uint16_t fdkDstPort = 40002;
uint16_t srcPort = 40003;                                         //source port
uint16_t ackPort = 40004;

UDPFrame fdkFrameHeader;

// service functions:
/* Compute checksum for count bytes starting at addr, using one's complement of one's complement sum*/
static uint16_t computeIpChecksum(uint8_t *addr, uint16_t count)
{
  uint32_t sum = 0;
  uint16_t word16 = 0;

  while(count > 1)
  {
    word16 = *addr<<8 | *(addr+1);
    sum += word16;
    count -= 2;
    addr += 2;
  }

  //if any bytes left, pad the bytes and add
  if(count > 0)
  {
    sum += ((*addr)<<8 | 0x00);
  }

  //Fold sum to 16 bits: add carrier to result
  while (sum>>16)
  {
      sum = (sum & 0xffff) + (sum >> 16);
  }
  //one's complement
  sum = ~sum;
  return ((uint16_t)sum);
}

uint16_t computeUdpChecksum(const UDPFrame* frameHeader, uint8_t *payloadAddr, uint16_t payloadSize)
{
    uint8_t frameBuf[256]= {0};

    memcpy(&(frameBuf[0]), frameHeader->structData.srcIpAddress, 4);
    memcpy(&(frameBuf[4]), frameHeader->structData.dstIpAddress, 4);
    frameBuf[9] = frameHeader->structData.protocol;

    uint16_t pseudoHdrLen = __builtin_bswap16(frameHeader->structData.ipTotalLength) - IP_HEADER_SIZE;
    frameBuf[10] = (pseudoHdrLen & 0xFF00) >> 8;
    frameBuf[11] = pseudoHdrLen & 0xFF;

    memcpy(&(frameBuf[12]), &(frameHeader->rawData[ETHERNETII_HEADER_SIZE+IP_HEADER_SIZE]), UDP_ONLY_HEADER_SIZE);
    memcpy(&(frameBuf[20]), payloadAddr, payloadSize);

    return computeIpChecksum(frameBuf, UDP_ONLY_HEADER_SIZE + UDP_PSEUDO_HEADER_SIZE + payloadSize);
}

int compareArrays(uint8_t a[], uint8_t b[], int n)
{
  for(uint8_t i = 1; i < n; i++)
  {
    if (a[i] != b[i]) return 0;
  }
  return 1;
}


void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void ETHERNET_Init()
{
    ETHDRV_GenerateMacAddr(MACAddr);                                //get the chip MAC address
    printf("mac addr:");
    for(uint8_t i = 0; i < 6; i++)
        printf("%x ",MACAddr[i]);
    printf("\r\n");

    TIM2_Init();

    ETHDRV_Init(IPAddr, GWIPAddr, IPMask, MACAddr);

    printf("IP addr: %d.%d.%d.%d\r\n", IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3]);

    ETH->MACA0HR = (uint32_t)MACAddr[5]<<8 | (uint32_t)MACAddr[4];
    ETH->MACA0LR = (uint32_t)MACAddr[3]<<24 |(uint32_t)MACAddr[2]<<16 |(uint32_t)MACAddr[1]<<8 |(uint32_t)MACAddr[0];
}

void ETHERNET_ParseArpFrame(const RecievedFrameData* frame)
{
    ARPFrame parsedFrame, answerFrame;

    memcpy(parsedFrame.rawData, frame, ARP_FULL_HEADER_SIZE);

    if(compareArrays(parsedFrame.structData.targetIpAdr, IPAddr, 4))
    {
        if(parsedFrame.structData.opCode == __builtin_bswap16(ARP_OPCODE_REQUEST))
        {
            answerFrame = parsedFrame;

            memcpy(answerFrame.structData.dstMAC, parsedFrame.structData.srcMAC, 6);
            memcpy(answerFrame.structData.srcMAC, MACAddr, 6);

            answerFrame.structData.opCode = __builtin_bswap16(ARP_OPCODE_REPLY);

            memcpy(answerFrame.structData.targetIpAdr, parsedFrame.structData.senderIpAdr, 4);
            memcpy(answerFrame.structData.senderIpAdr, IPAddr, 4);

            memcpy(answerFrame.structData.targetHwAdr, parsedFrame.structData.senderHwAdr, 6);
            memcpy(answerFrame.structData.senderHwAdr, MACAddr, 6);

            ETH_TxPktChainMode(ARP_FULL_HEADER_SIZE, answerFrame.rawData);
        }
    }
}

void ETHERNET_ParseIcmpFrame(const RecievedFrameData* frame)
{
    ICMPFrame parsedFrame, answerFrame;

    memcpy(parsedFrame.rawData, frame, ICMP_FULL_HEADER_SIZE);

    if(parsedFrame.structData.icmpType == ICMP_TYPE_ECHO_REQUEST)
    {
        if(compareArrays(parsedFrame.structData.dstIpAddress, IPAddr, 4))
        {
            answerFrame = parsedFrame;

            uint16_t answerLength = ICMP_FULL_HEADER_SIZE;

            memcpy(answerFrame.structData.srcMAC, MACAddr, 6);
            memcpy(answerFrame.structData.dstMAC, parsedFrame.structData.srcMAC, 6);

            memcpy(answerFrame.structData.srcIpAddress, IPAddr, 4);
            memcpy(answerFrame.structData.dstIpAddress, parsedFrame.structData.srcIpAddress, 4);

            answerFrame.structData.ipTotalLength = __builtin_bswap16(answerLength - ETHERNETII_HEADER_SIZE);

            answerFrame.structData.checksum = 0;
            uint16_t checkSumIp = computeIpChecksum(&(answerFrame.rawData[ETHERNETII_HEADER_SIZE]), IP_HEADER_SIZE);
            answerFrame.structData.checksum = __builtin_bswap16(checkSumIp);

            answerFrame.structData.icmpType = ICMP_TYPE_ECHO_ANSWER;

            answerFrame.structData.icmpChecksum = 0;
            uint16_t checksum = computeIpChecksum(&(answerFrame.rawData[ETHERNETII_HEADER_SIZE + IP_HEADER_SIZE]),
                                                    ICMP_FULL_HEADER_SIZE - ETHERNETII_HEADER_SIZE - IP_HEADER_SIZE);
            answerFrame.structData.icmpChecksum = __builtin_bswap16(checksum);

            ETH_TxPktChainMode(answerLength, answerFrame.rawData);
        }
    }
}

void ETHERNET_ParseUdpFrame(const RecievedFrameData* frame)
{
    UDPFrame parsedFrameHeader, answerFrameHeader;

    memcpy(parsedFrameHeader.rawData, frame, UDP_FULL_HEADER_SIZE);

    uint8_t srcIp[4] = {0};
    uint8_t dstIp[4] = {0};

    memcpy(srcIp, parsedFrameHeader.structData.srcIpAddress, 4);
    memcpy(dstIp, parsedFrameHeader.structData.dstIpAddress, 4);

    if(compareArrays(dstIp, IPAddr, 4))
    {
        answerFrameHeader = parsedFrameHeader;


        memcpy(mstMACAddr, parsedFrameHeader.structData.srcMAC, 6);
        memcpy(mstIPAddr, parsedFrameHeader.structData.srcIpAddress, 4);

        uint8_t answer[512];
        uint32_t outDataLen;


        parseFrame(&(frame->frameData[UDP_PAYLOAD_POSITION]), __builtin_bswap16(parsedFrameHeader.structData.udpLength) - UDP_ONLY_HEADER_SIZE, &(answer[UDP_PAYLOAD_POSITION]), &outDataLen);

        if(outDataLen > 0)
        {
            uint16_t totalAnswerLen = UDP_FULL_HEADER_SIZE+outDataLen;

            memcpy(answerFrameHeader.structData.srcMAC, MACAddr, 6);
            memcpy(answerFrameHeader.structData.dstMAC, parsedFrameHeader.structData.srcMAC, 6);

            answerFrameHeader.structData.ipTotalLength = __builtin_bswap16(totalAnswerLen - ETHERNETII_HEADER_SIZE);

            memcpy(answerFrameHeader.structData.srcIpAddress, IPAddr, 4);
            memcpy(answerFrameHeader.structData.dstIpAddress, parsedFrameHeader.structData.srcIpAddress, 4);

            answerFrameHeader.structData.srcPort = __builtin_bswap16(srcPort);
//            answerFrameHeader.structData.dstPort = parsedFrameHeader.structData.srcPort;
            answerFrameHeader.structData.dstPort = __builtin_bswap16(ackPort);

            answerFrameHeader.structData.udpLength = __builtin_bswap16(outDataLen + UDP_ONLY_HEADER_SIZE);

            answerFrameHeader.structData.checksum = 0;
            uint16_t checkSumIp = computeIpChecksum(&(answerFrameHeader.rawData[ETHERNETII_HEADER_SIZE]), IP_HEADER_SIZE);
            answerFrameHeader.structData.checksum = __builtin_bswap16(checkSumIp);

            answerFrameHeader.structData.udpCheckSum = 0;
            /* commented in case of perfomance. Computing UDP checksum request 10us. All other operations reques 15us
            uint16_t checkSumUdp = computeUdpChecksum(&answerFrameHeader, &(answer[UDP_PAYLOAD_POSITION]), outDataLen);
            answerFrameHeader.structData.udpCheckSum = __builtin_bswap16(checkSumUdp);
            */

            memcpy(answer, answerFrameHeader.rawData, UDP_FULL_HEADER_SIZE);

            ETH_TxPktChainMode(totalAnswerLen, answer);

            if(framesCounter == 255) framesCounter = 0;
            else framesCounter++;
        }
    }
}

void ETHERNET_SendFdkFrame()
{
    uint8_t dummyMACAddr[6] = {0};

    if(!compareArrays(mstMACAddr, dummyMACAddr, 6))
    {
        uint8_t rawFdkFrame[512] = {0};
        uint16_t payloadLen;

        getFdkPayload(&(rawFdkFrame[UDP_PAYLOAD_POSITION]), &payloadLen);

        uint16_t totalAnswerLen = UDP_FULL_HEADER_SIZE+payloadLen;

        memcpy(fdkFrameHeader.structData.srcMAC, MACAddr, 6);
        memcpy(fdkFrameHeader.structData.dstMAC, mstMACAddr, 6);

        fdkFrameHeader.structData.frameType = __builtin_bswap16(FRAME_TYPE_IPv4);
        fdkFrameHeader.structData.ipVerHdrLen = 0x45;
        fdkFrameHeader.structData.diffServicesField = 0x00;

        fdkFrameHeader.structData.ipTotalLength = __builtin_bswap16(totalAnswerLen - ETHERNETII_HEADER_SIZE);

        fdkFrameHeader.structData.identification = 0x0000;

        fdkFrameHeader.structData.fragmentFlagsAndOffset = 0x40; // not fragmented

        fdkFrameHeader.structData.ttl = 0xFF;
        fdkFrameHeader.structData.protocol = IPv4_PROTOCOL_UDP;

        memcpy(fdkFrameHeader.structData.srcIpAddress, IPAddr, 4);
        memcpy(fdkFrameHeader.structData.dstIpAddress, mstIPAddr, 4);

        fdkFrameHeader.structData.srcPort = __builtin_bswap16(srcPort);
        fdkFrameHeader.structData.dstPort = __builtin_bswap16(fdkDstPort);

        fdkFrameHeader.structData.udpLength = __builtin_bswap16(payloadLen + UDP_ONLY_HEADER_SIZE);

        fdkFrameHeader.structData.checksum = 0;
        uint16_t checkSumIp = computeIpChecksum(&(fdkFrameHeader.rawData[ETHERNETII_HEADER_SIZE]), IP_HEADER_SIZE);
        fdkFrameHeader.structData.checksum = __builtin_bswap16(checkSumIp);

        fdkFrameHeader.structData.udpCheckSum = 0;

        memcpy(rawFdkFrame, fdkFrameHeader.rawData, UDP_FULL_HEADER_SIZE);

        ETH_TxPktChainMode(totalAnswerLen, rawFdkFrame);

        if(framesCounter == 255) framesCounter = 0;
        else framesCounter++;
    }
}
