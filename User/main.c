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
void TIM2_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void TIM3_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

bool ledState=0;

// ----> to pilot_signal.h OR hum.h
ControlPin_t pinHumSW;
ControlPin_t pinHumOn;
ControlPin_t pinVgNeg1;
ControlPin_t pinVgNeg2;

ControlPin_t pinVC1;
ControlPin_t pinVC2;

ControlPin_t pinComPs;

typedef enum
{
    PS_OFF = 0,
    PS_SIN,
    PS_HUM,
    PS_LCM
}PILOT_type_t;

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

    // HUM
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    pinHumSW.pin = GPIO_Pin_1; // FIlter pi actual!!!!
    pinHumSW.port = GPIOD;

    GPIO_InitStructure.GPIO_Pin = pinHumSW.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinHumSW.port, &GPIO_InitStructure);

    pinHumOn.pin = GPIO_Pin_2;
    pinHumOn.port = GPIOD;

    GPIO_InitStructure.GPIO_Pin = pinHumOn.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinHumOn.port, &GPIO_InitStructure);

    // VC
    pinVC1.pin = GPIO_Pin_3;
    pinVC1.port = GPIOD;

    GPIO_InitStructure.GPIO_Pin = pinVC1.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinVC1.port, &GPIO_InitStructure);

    pinVC2.pin = GPIO_Pin_4;
    pinVC2.port = GPIOD;

    GPIO_InitStructure.GPIO_Pin = pinVC2.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinVC2.port, &GPIO_InitStructure);

    // Vg
    pinVgNeg1.pin = GPIO_Pin_3;
    pinVgNeg1.port = GPIOB;

    GPIO_InitStructure.GPIO_Pin = pinVgNeg1.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinVgNeg1.port, &GPIO_InitStructure);

    pinVgNeg2.pin = GPIO_Pin_4;
    pinVgNeg2.port = GPIOB;

    GPIO_InitStructure.GPIO_Pin = pinVgNeg2.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinVgNeg2.port, &GPIO_InitStructure);

    // ComPS
    pinComPs.pin = GPIO_Pin_0;
    pinComPs.port = GPIOC;

    GPIO_InitStructure.GPIO_Pin = pinComPs.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(pinComPs.port, &GPIO_InitStructure);
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

    NVIC_EnableIRQ(EXTI0_IRQn);
}

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 10 * SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 10000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseStructure.TIM_RepetitionCounter = 1000;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

//uint64_t numpCounter = 0;
//uint64_t errTVRS, errNUMP;
//bool warning=false;

volatile bool flagSendFdk = 0;
volatile bool flagSendRdy = 0;
bool flagSetHeterodine = 0;
int main(void)
{
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);

    LFM_Init();
    HET_Init();

    PIN_Init();

    CommFIFO_Init();

    printf("UDP client. Recieving control frames for FSGP\r\n");
    printf("SystemClk: %d\r\n",SystemCoreClock);
    printf("ChipID: %08x\r\n", DBGMCU_GetCHIPID());

    Delay_Ms(10000);

	LFM_WriteStartupData();
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

    Delay_Ms(2000);

//    bool warningWritten = false;

	while(1)
    {
//	    if(!warningWritten)
//	    {
//	        if(warning)
//            {
//	            warningWritten = true;
//	            printf("TVRS and NUMP error: %d %d", errTVRS, errNUMP);
//            }
//	    }

        ETHDRV_MainTask();

        if(flagSetHeterodine && CommFIFO_Count()>0)
        {
            HET_SetHeterodine(CommFIFO_PeekData().rcvdFrame.NKCH);
            flagSetHeterodine = 0;
        }

        while(EthFIFO_Count()>0){
            RecievedDataPtr_t* data = EthFIFO_GetData();
//            printf("rec data:%d bytes\r\n", data->frameLength);
//            printf("data:%x\r\n", data->bufferPtr);


            if(data->frameLength>0)
            {
    //            printf("Recieved frame length: %d\r\n", recievedFrameData.frameLength);
                RecievedFrameData recievedFrameDataSaved;
                memcpy(&recievedFrameDataSaved, data->bufferPtr, data->frameLength);

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
            }
        }

        if(flagSendFdk)
        {
            ETHERNET_SendFdkFrame();

            isRecievingControlFrames = 0;
            flagSendFdk = 0;
        }

        if(flagSendRdy)
        {
            ETHERNET_SendRdyFrame();
            isRecievingControlFrames = 0;
            flagSendRdy = 0;
        }
	}
}

// IRQ handlers ======================
uint16_t repCounter = 0;
void TIM3_IRQHandler()
{
    repCounter++;

    if(repCounter == 5){
        flagSendRdy = 1;
    }

    if(repCounter == 10){
        flagSendFdk = 1;
        repCounter = 0;
    }

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

void EXTI0_IRQHandler(void)
{
//    numpCounter++;
    actualComm = CommFIFO_GetData();

    if(actualComm)
    {
        GPIO_SetBits(pinVC1.port, pinVC1.pin);

        HET_SetFilters(actualComm->rcvdFrame.NKCH);

//        if(numpCounter != actualComm->rcvdFrame.TVRS)
//        {
//            warning = true;
//            errTVRS = actualComm->rcvdFrame.TVRS;
//            errNUMP = numpCounter;
//        }

        // зг3здзв03
        if(actualComm->rcvdFrame.NKCH < 36)
        {
//            GPIO_SetBits(pinVC1.port, pinVC1.pin);
            GPIO_SetBits(pinVC2.port, pinVC2.pin);
        }
        else if(actualComm->rcvdFrame.NKCH >= 36 && actualComm->rcvdFrame.NKCH < 51)
        {
//            GPIO_ResetBits(pinVC1.port, pinVC1.pin);
            GPIO_SetBits(pinVC2.port, pinVC2.pin);
        }
        else
        {
//            GPIO_SetBits(pinVC1.port, pinVC1.pin);
            GPIO_ResetBits(pinVC2.port, pinVC2.pin);
        }

        GPIO_WriteBit(pinComPs.port, pinComPs.pin, actualComm->rcvdFrame.ComPS);

        switch(actualComm->rcvdFrame.TipPS)
        {
        case PS_OFF:
        {
            GPIO_SetBits(pinHumOn.port, pinHumOn.pin);
            GPIO_SetBits(pinHumSW.port, pinHumSW.pin);
            GPIO_ResetBits(pinVgNeg1.port, pinVgNeg1.pin);
            GPIO_SetBits(pinVgNeg2.port, pinVgNeg2.pin);
            break;
        }
        case PS_SIN:
        {
            GPIO_SetBits(pinHumOn.port, pinHumOn.pin);
            GPIO_SetBits(pinHumSW.port, pinHumSW.pin);
            GPIO_SetBits(pinVgNeg1.port, pinVgNeg1.pin);
            GPIO_ResetBits(pinVgNeg2.port, pinVgNeg2.pin);
            break;
        }
        case PS_HUM:
        {
            GPIO_ResetBits(pinHumOn.port, pinHumOn.pin);
            GPIO_ResetBits(pinHumSW.port, pinHumSW.pin);
            GPIO_SetBits(pinVgNeg1.port, pinVgNeg1.pin);
            GPIO_ResetBits(pinVgNeg2.port, pinVgNeg2.pin);
            break;
        }
        case PS_LCM:
        {
            GPIO_SetBits(pinHumOn.port, pinHumOn.pin);
            GPIO_SetBits(pinHumSW.port, pinHumSW.pin);
            GPIO_SetBits(pinVgNeg1.port, pinVgNeg1.pin);
            GPIO_ResetBits(pinVgNeg2.port, pinVgNeg2.pin);
            break;
        }
        }

        LFM_SetPack(&actualComm->ddsData);

        HET_UpdateIO();
        flagSetHeterodine = 1;
    }

//    printf("used nk4:%d\r\n", actualComm->NKCH);

    GPIO_ResetBits(pinVC1.port, pinVC1.pin);
    EXTI_ClearITPendingBit(EXTI_Line0);
}
