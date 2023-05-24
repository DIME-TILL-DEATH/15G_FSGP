/******************************************
* File Name          : main.c
* Author             : Dmitriy Kostyuchik

* Description        : Main program body
*******************************************/

#include "string.h"
#include "debug.h"

#include "uart.h"
#include "ethernet.h"

#include "command_fifo.h"
#include "frame_parser.h"

int main(void)
{
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);

	printf("UDP client. Recieving control frames for FSGP\r\n");
	printf("SystemClk: %d\r\n",SystemCoreClock);
	printf( "ChipID: %08x\r\n", DBGMCU_GetCHIPID() );

	ETHERNET_Init(parse_frame);
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
