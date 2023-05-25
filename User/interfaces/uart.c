/*
 * uart.c
 *
 *  Created on: May 19, 2023
 *      Author: Dmitriy Kosiuchik
 */
#include "uart.h"
#include <string.h>

#define UART_BUFFER_SIZE 256

u8 txBuffer1[UART_BUFFER_SIZE] = {0};
u8 txBuffer2[UART_BUFFER_SIZE] = {0};
u8 rxBuffer1[UART_BUFFER_SIZE] = {0};
u8 rxBuffer2[UART_BUFFER_SIZE] = {0};

void DMA1_Channel6_IRQHandler() __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));
void DMA1_Channel3_IRQHandler() __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

void UART_DMA_Init()
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Common
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)txBuffer1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = UART_BUFFER_SIZE;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)rxBuffer1;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART_BUFFER_SIZE;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuffer2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = UART_BUFFER_SIZE;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)rxBuffer2;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART_BUFFER_SIZE;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    DMA_Cmd(DMA1_Channel6, ENABLE); /* USART2 Rx */
    DMA_Cmd(DMA1_Channel7, ENABLE); /* USART2 Tx */
    DMA_Cmd(DMA1_Channel2, ENABLE); /* USART3 Tx */
    DMA_Cmd(DMA1_Channel3, ENABLE); /* USART3 Rx */
}

void UART_Init(void)
{
    UART_DMA_Init();

    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);

    USART_InitStructure.USART_BaudRate = 2000000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Init(USART3, &USART_InitStructure);

    USART_DMACmd(USART2, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

void UART_WriteData(UART_Type uartType, uint8_t* data_ptr, uint16_t len)
{
    switch (uartType)
    {
        case UART_NUM1:
            DMA_SetCurrDataCounter(DMA1_Channel7, len);
            DMA_SetCurrDataCounter(DMA1_Channel6, len);
            memcpy(txBuffer1, data_ptr, len);
            break;
        case UART_NUM2:
            DMA_SetCurrDataCounter(DMA1_Channel2, len);
            DMA_SetCurrDataCounter(DMA1_Channel3, len);
            memcpy(txBuffer2, data_ptr, len);
            break;
        default:
            break;
    }
}

void UART_WriteByte(UART_Type uartType, uint8_t data)
{
    switch (uartType)
    {
        case UART_NUM1:
            USART2->DATAR = data;
            break;
        case UART_NUM2:
            USART3->DATAR = data;
            break;
        default:
            break;
    }
}

void DMA1_Channel6_IRQHandler()
{
    DMA_ClearITPendingBit(DMA1_IT_GL6);
    printf("DMA rx INT\r\n");
}

void DMA1_Channel3_IRQHandler()
{
    DMA_ClearITPendingBit(DMA1_IT_GL3);
}
