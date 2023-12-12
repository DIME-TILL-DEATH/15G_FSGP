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
#include "hetFormer.h"
#include "spi_heterodine.h"

#include "command_fifo.h"
#include "frame_parser.h"

void EXTI0_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));

bool ledState=0;
uint8_t framesCounter = 0;

void PIN_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};


    // NP
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Strobe
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
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

//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource14); //PA14 - STROBE
//    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);

    NVIC_EnableIRQ(EXTI0_IRQn);
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
}

bool flagSendFdk = 0;
bool flagSetHeterodine = 0;
int main(void)
{
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);

    printf("UDP client. Recieving control frames for FSGP\r\n");
    printf("SystemClk: %d\r\n",SystemCoreClock);
    printf("ChipID: %08x\r\n", DBGMCU_GetCHIPID());

    Delay_Ms(1000);

	CommFIFO_Init();

	LFM_Init();
	HET_Init();

	PIN_Init();
	ETHERNET_Init();

    TIM3_Init();
    INT_Init();

    NVIC_SetPriority(TIM3_IRQn,     (4<<5) | (0x01<<4));/* Group priority 3, lower overall priority */
    NVIC_SetPriority(ETH_IRQn,      (3<<5) | (0x01<<4));
    //NVIC_SetPriority(EXTI15_10_IRQn,(2<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI0_IRQn,    (0<<5) | (0x01<<4));
    //NVIC_SetPriority(SPI3_IRQn,     (1<<5) | (0x01<<4));/* Group priority 0, overall priority is higher */

    printf("NVIC priorities SPI3: %x, EXTI: %x, ETH: %x, TIM3: %x\r\n",
            NVIC->IPRIOR[SPI3_IRQn], NVIC->IPRIOR[EXTI0_IRQn], NVIC->IPRIOR[ETH_IRQn], NVIC->IPRIOR[TIM3_IRQn]);

    // Enable IRQ's only when all initiallization finished
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);

    //Delay_Ms(2000);

	while(1)
    {
        ETHDRV_MainTask();

        if(flagSendFdk)
        {
            ETHERNET_SendFdkFrame();
            flagSendFdk = 0;
        }

        if(flagSetHeterodine && CommFIFO_Count()>0)
        {
            HET_SetHeterodine(CommFIFO_PeekData().NKCH);
            flagSetHeterodine = 0;
        }

        if(recievedFrameData.frameLength>0)
        {
            NVIC_DisableIRQ(TIM3_IRQn);

            GPIO_SetBits(GPIOC, GPIO_Pin_3);
            RecievedFrameData recievedFrameDataSaved;
            memcpy(&recievedFrameDataSaved, &recievedFrameData, recievedFrameData.frameLength);

            uint16_t frameType = recievedFrameDataSaved.frameData[POS_FRAME_TYPE_HW]<<8 | recievedFrameDataSaved.frameData[POS_FRAME_TYPE_LW];
            uint8_t ipProtocolType = recievedFrameDataSaved.frameData[POS_PROTOCOL];

            switch(frameType)
            {
                case FRAME_TYPE_ARP:
                {
                    ETHERNET_ParseArpFrame(&recievedFrameDataSaved);
                    break;
                }

                case FRAME_TYPE_IPv4:
                {
                    switch(ipProtocolType)
                    {
                    case IPv4_PROTOCOL_UDP: ETHERNET_ParseUdpFrame(&recievedFrameDataSaved); break;
                    case IPv4_PROTOCOL_ICMP: ETHERNET_ParseIcmpFrame(&recievedFrameDataSaved); break;
                    }
                    break;
                }
            }
            recievedFrameData.frameLength = 0;
            NVIC_EnableIRQ(TIM3_IRQn);

            GPIO_ResetBits(GPIOC, GPIO_Pin_3);
        }
	}
}

// IRQ handlers ======================
void TIM3_IRQHandler()
{
    flagSendFdk = 1;

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

void EXTI0_IRQHandler(void)
{
    GPIOC->BSHR = GPIO_Pin_2;

    FSGP_Command_Frame* actualComm = CommFIFO_GetData();

    HET_UpdateIO();

    LFM_SetPack(actualComm->KP);

    HET_SetFilters(actualComm->NKCH);

    flagSetHeterodine = 1;

    GPIOC->BCR = GPIO_Pin_2;

    EXTI_ClearITPendingBit(EXTI_Line0);
}
