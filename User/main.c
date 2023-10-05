/******************************************
* File Name          : main.c
* Author             : Dmitriy Kostyuchik

* Description        : Main program body
*******************************************/

#include "string.h"
#include "debug.h"

#include "uart.h"
#include "ethernet.h"

#include "lfmFormer.h"

#include "command_fifo.h"
#include "frame_parser.h"

void EXTI0_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
//void EXTI15_10_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void TIM3_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

bool ledState=0;
uint8_t framesCounter = 0;

void PIN_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    // UART pins
    GPIO_PinRemapConfig(GPIO_Remap_USART2 | GPIO_FullRemap_USART3, ENABLE);

    /* USART2 remap TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* USART3 remap TX_1-->D.8  RX_1-->D.9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Free pins
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Key - NP & STROBE
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void INT_Init()
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //PA0 - NP
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource14); //PA14 - STROBE
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);

//    NVIC_EnableIRQ(EXTI0_IRQn);
//    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock;
    TIM_TimeBaseStructure.TIM_Prescaler = 10000; //2880;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//    NVIC_EnableIRQ(TIM3_IRQn);
}

int main(void)
{
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);

	LFM_Init();

	printf("UDP client. Recieving control frames for FSGP\r\n");
	printf("SystemClk: %d\r\n",SystemCoreClock);
	printf( "ChipID: %08x\r\n", DBGMCU_GetCHIPID() );

	PIN_Init();
	ETHERNET_Init();

    UART_Init();
    TIM3_Init();
    INT_Init();

//    LFM_Init();

    NVIC_SetPriority(TIM3_IRQn,   (3<<5) | (0x01<<4));/* Group priority 3, lower overall priority */
    NVIC_SetPriority(ETH_IRQn, (2<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI15_10_IRQn, (1<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI0_IRQn, (0<<5) | (0x01<<4));/* Group priority 0, overall priority is higher */

    printf("NVIC priorities EXTI: %x, ETH: %x, TIM3: %x\r\n", NVIC->IPRIOR[EXTI0_IRQn], NVIC->IPRIOR[ETH_IRQn], NVIC->IPRIOR[TIM3_IRQn]);

    // Enable IRQ's only when all initiallization finished
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);

	while(1)
    {
        ETHDRV_MainTask();

        if(recievedFrameData.frameLength>0)
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_3);

            uint16_t frameType = recievedFrameData.frameData[POS_FRAME_TYPE_HW]<<8 | recievedFrameData.frameData[POS_FRAME_TYPE_LW];
            uint8_t ipProtocolType = recievedFrameData.frameData[POS_PROTOCOL];

            switch(frameType)
            {
            case FRAME_TYPE_ARP:
                ETHERNET_ParseArpFrame(&recievedFrameData);
                break;

            case FRAME_TYPE_IPv4:
                switch(ipProtocolType)
                {
                case IPv4_PROTOCOL_UDP: ETHERNET_ParseUdpFrame(&recievedFrameData); break;
                case IPv4_PROTOCOL_ICMP: ETHERNET_ParseIcmpFrame(&recievedFrameData); break;
                }
                break;
            }
            recievedFrameData.frameLength = 0;

            GPIO_ResetBits(GPIOC, GPIO_Pin_3);
        }
	}
}

// IRQ handlers
void TIM3_IRQHandler()
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_10, (ledState == 0) ? (ledState = Bit_SET) : (ledState = Bit_RESET));

    if(CommFIFO_Count() == 0)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_11, Bit_SET);
    }
    else
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_11, Bit_RESET);
    }

    ETHERNET_SendFdkFrame();

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

uint8_t bufData[4] = {1, 2, 3, 4};
void EXTI0_IRQHandler(void)
{
    GPIOC->BSHR = GPIO_Pin_2;

    FSGP_Command_Frame* actualComm = CommFIFO_GetData();

    LFM_SetPack(actualComm->KP);

    GPIOC->BCR = GPIO_Pin_2;

    EXTI_ClearITPendingBit(EXTI_Line0);
}
