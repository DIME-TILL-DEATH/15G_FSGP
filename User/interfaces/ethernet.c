#include "string.h"

#include "ethernet.h"

//void ETHERNET_UdpRecieve(struct _SCOK_INF *socinf, u32 ipaddr, u16 port, u8 *buf, u32 len);

#define UDP_REC_BUF_LEN                1472
uint8_t MACAddr[6];                                              //MAC address
uint8_t IPAddr[4] = {192, 168, 104, 10};                         //IP address
uint8_t GWIPAddr[4] = {192, 168, 104, 255};                        //Gateway IP address
uint8_t IPMask[4] = {255, 255, 255, 0};                        //subnet mask
uint8_t DESIP[4] = {0, 0, 0, 0};                         //destination IP address
uint16_t desport = 40003;                                         //destination port
uint16_t srcport = 40003;                                         //source port

// service functions:
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

    ETH->MACA0HR = (uint32_t)MACAddr[5]<<8 | (uint32_t)MACAddr[4];
    ETH->MACA0LR = (uint32_t)MACAddr[3]<<24 |(uint32_t)MACAddr[2]<<16 |(uint32_t)MACAddr[1]<<8 |(uint32_t)MACAddr[0];
}

/*********************************************************************
 * @fn      ETHERNET_UdpServerRecv
 *
 * @brief   UDP Receive data function
 *
 *@param    socinf - socket information.
 *          ipaddr - The IP address from which the data was sent
 *          port - source port
 *          buf - pointer to the data buffer
 *          len - received data length
 * @return  none
 */
//void ETHERNET_UdpRecieve(struct _SCOK_INF *socinf, u32 ipaddr, u16 port, u8 *buf, u32 len)
//{
//    GPIO_SetBits(GPIOC, GPIO_Pin_3);
//    GPIO_SetBits(GPIOC, GPIO_Pin_2);
//
//    u8 ip_addr[4], i;
//
//    uint32_t outDataLen;
//
//    parse_frame_func(buf, len, buf, &outDataLen);
//    GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//
//    WCHNET_SocketUdpSendTo(socinf->SockIndex, buf, &outDataLen, ip_addr, port);
//    GPIO_ResetBits(GPIOC, GPIO_Pin_3);
//
//    printf("Rm IP: ");
//    for (i = 0; i < 4; i++) {
//        ip_addr[i] = ipaddr & 0xff;
//        printf("%d ", ip_addr[i]);
//        ipaddr = ipaddr >> 8;
//    }
//
//    printf("port = %d len = %d\r\n", port, len);
//}

void ETHERNET_ParseUdpFrame(const RecievedFrameData* frame)
{
    printf("UDP frame recieved\r\n");
}

void ETHERNET_ParseIcmpFrame(const RecievedFrameData* frame)
{
    printf("ICMP frame recieved\r\n");
}

void ETHERNET_ParseArpFrame(const RecievedFrameData* frame)
{
    ARPFrame parsedFrame, answerFrame;

    memcpy(parsedFrame.rawData, frame, ARP_FRAME_SIZE);

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

            ETH_TxPktChainMode(ARP_FRAME_SIZE, answerFrame.rawData);
        }
    }
}
