#include "string.h"

#include "ethernet.h"

// Only for this file
static void mStopIfError(u8 iError);
void ETHERNET_CreateUdpSocket(void);
void ETHERNET_UdpRecieve(struct _SCOK_INF *socinf, u32 ipaddr, u16 port, u8 *buf, u32 len);

#define UDP_REC_BUF_LEN                1472
u8 MACAddr[6];                                              //MAC address
u8 IPAddr[4] = { 192, 168, 104, 10 };                         //IP address
u8 GWIPAddr[4] = { 192, 168, 104, 255 };                        //Gateway IP address
u8 IPMask[4] = { 255, 255, 255, 0 };                        //subnet mask
u8 DESIP[4] = { 0, 0, 0, 0 };                         //destination IP address
u16 desport = 40003;                                         //destination port
u16 srcport = 40003;                                         //source port

u8 SocketId;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_REC_BUF_LEN];      //socket receive buffer
u8 MyBuf[UDP_REC_BUF_LEN];

parser_ptr parse_frame_func;

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

void ETHERNET_Init(parser_ptr func)
{
    ETHDRV_GetMacAddr(MACAddr);                                //get the chip MAC address
    printf("mac addr:");
    for(uint8_t i = 0; i < 6; i++)
        printf("%x ",MACAddr[i]);
    printf("\r\n");

    TIM2_Init();

    uint8_t result = ETHDRV_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);        //Ethernet library initialize
//    mStopIfError(result);


//    if (result == WCHNET_ERR_SUCCESS)
//        printf("WCHNET_LibInit Success\r\n");
//
//    for (u8 i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
//        ETHERNET_CreateUdpSocket();

    parse_frame_func = func;
}

void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

void ETHERNET_CreateUdpSocket(void)
{
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[SocketId];
    TmpSocketInf.RecvBufLen = UDP_REC_BUF_LEN;
    TmpSocketInf.AppCallBack = ETHERNET_UdpRecieve;
    u8 result = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("WCHNET_SocketCreat %d\r\n", SocketId);
    mStopIfError(result);
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
void ETHERNET_UdpRecieve(struct _SCOK_INF *socinf, u32 ipaddr, u16 port, u8 *buf, u32 len)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_3);
    GPIO_SetBits(GPIOC, GPIO_Pin_2);

    u8 ip_addr[4], i;

    uint32_t outDataLen;

    parse_frame_func(buf, len, buf, &outDataLen);
    GPIO_ResetBits(GPIOC, GPIO_Pin_2);

    WCHNET_SocketUdpSendTo(socinf->SockIndex, buf, &outDataLen, ip_addr, port);
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);

    printf("Rm IP: ");
    for (i = 0; i < 4; i++) {
        ip_addr[i] = ipaddr & 0xff;
        printf("%d ", ip_addr[i]);
        ipaddr = ipaddr >> 8;
    }

    printf("port = %d len = %d\r\n", port, len);
}

void ETHERNET_HandleGlobalInt(void)
{
    u8 intState = WCHNET_GetGlobalInt();                       //get global interrupt flag
    if (intState & GINT_STAT_UNREACH)                          //Unreachable interrupt
    {
        printf("GINT_STAT_UNREACH\r\n");
    }

    if (intState & GINT_STAT_IP_CONFLI)                        //IP conflict
    {
        printf("GINT_STAT_IP_CONFLI\r\n");
    }

    if (intState & GINT_STAT_PHY_CHANGE)                       //PHY status change
    {
        u16 phyStatus = WCHNET_GetPHYStatus();
        if (phyStatus & PHY_Linked_Status)
            printf("PHY Link Success\r\n");
        else
            printf("PHY Link lost\r\n");
    }
}
