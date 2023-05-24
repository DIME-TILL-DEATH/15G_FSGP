/******************************************
* File Name          : main.c
* Author             : Dmitriy Kostyuchik

* Description        : Main program body
*******************************************/

#include "string.h"
#include "debug.h"
#include "wchnet.h"
#include "eth_driver.h"

#include "uart.h"

#include "command_fifo.h"

#define UDP_RECE_BUF_LEN                1472
u8 MACAddr[6];                                              //MAC address
u8 IPAddr[4] = { 192, 168, 104, 10 };                         //IP address
u8 GWIPAddr[4] = { 192, 168, 104, 255 };                        //Gateway IP address
u8 IPMask[4] = { 255, 255, 255, 0 };                        //subnet mask
u8 DESIP[4] = { 0, 0, 0, 0 };                         //destination IP address
u16 desport = 40003;                                         //destination port
u16 srcport = 40003;                                         //source port

u8 SocketId;
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][UDP_RECE_BUF_LEN];      //socket receive buffer
u8 MyBuf[UDP_RECE_BUF_LEN];


void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
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

u8 frameNum = 0;

/*********************************************************************
 * @fn      WCHNET_UdpServerRecv
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
void WCHNET_UdpServerRecv(struct _SCOK_INF *socinf, u32 ipaddr, u16 port, u8 *buf, u32 len)
{
    u8 ip_addr[4], i;

    printf("Rm IP: ");
    for (i = 0; i < 4; i++) {
        ip_addr[i] = ipaddr & 0xff;
        printf("%d ", ip_addr[i]);
        ipaddr = ipaddr >> 8;
    }

    printf("port = %d len = %d\r\n", port, len);

    uint8_t data[2048] = {0};
    memcpy(data, buf, len);

    if(buf[COMMAND_FRAME_POS] == COMMAND_FRAME)
    {
        Command_Frame *comand_ptr = (Command_Frame *)&(data[COMMAND_DATA_POS]);
        Command_Frame recieved_command = *comand_ptr;

        convertEndians(&recieved_command);

        if(comm_fifo_putdata(recieved_command))
        {
            data[BUFFER_SIZE_LW_POS] = COMMAND_FIFO_SIZE;
            data[QUEUE_SIZE_LW_POS] = comm_fifo_count();
        }
        printf("Timestamp_lw: %X Index: %d TVRS: %d Command buffer: %d\r\n", recieved_command.timestamp_lw,
                                                                             recieved_command.index,
                                                                             recieved_command.TVRS,
                                                                             comm_fifo_count());
    }

    uint32_t sentLen = 32*4;

    WCHNET_SocketUdpSendTo(socinf->SockIndex, data, &sentLen, ip_addr, port);

    frameNum++;
    uart_write_data(UART_NUM1, &frameNum, 1);
    uart_write_data(UART_NUM2, &frameNum, 1);
}

void WCHNET_CreateUdpSocket(void)
{
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, DESIP, 4);
    TmpSocketInf.DesPort = desport;
    TmpSocketInf.SourPort = srcport;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[SocketId];
    TmpSocketInf.RecvBufLen = UDP_RECE_BUF_LEN;
    TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;
    u8 result = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    printf("WCHNET_SocketCreat %d\r\n", SocketId);
    mStopIfError(result);
}

void WCHNET_HandleGlobalInt(void)
{
    u8 intState = WCHNET_GetGlobalInt();                          //get global interrupt flag
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

int main(void)
{
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);

	printf("UDP client. Recieving control frames for FSGP\r\n");
	printf("SystemClk: %d\r\n",SystemCoreClock);
	printf( "ChipID: %08x\r\n", DBGMCU_GetCHIPID() );


    WCHNET_GetMacAddr(MACAddr);                                //get the chip MAC address
    printf("mac addr:");
    for(u8 i = 0; i < 6; i++)
        printf("%x ",MACAddr[i]);
    printf("\r\n");

    TIM2_Init();

    u8 result = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);        //Ethernet library initialize
    mStopIfError(result);

    if (result == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");

    for (u8 i = 0; i < WCHNET_MAX_SOCKET_NUM; i++)
        WCHNET_CreateUdpSocket();

    uart_init();

	while(1)
    {
        WCHNET_MainTask();
        if(WCHNET_QueryGlobalInt())
        {
            WCHNET_HandleGlobalInt();
        }
	}
}
