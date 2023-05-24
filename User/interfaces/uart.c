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

void DMA_for_uart_init()
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
}

void uart_init(void)
{
    DMA_for_uart_init();

    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_USART2 | GPIO_FullRemap_USART3, ENABLE);

    /* USART2 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* USART3 TX_1-->B.18  RX_1-->B.11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 2000000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Init(USART3, &USART_InitStructure);

    DMA_Cmd(DMA1_Channel6, ENABLE); /* USART1 Tx */
    DMA_Cmd(DMA1_Channel7, ENABLE); /* USART1 Rx */
    DMA_Cmd(DMA1_Channel2, ENABLE); /* USART2 Tx */
    DMA_Cmd(DMA1_Channel3, ENABLE); /* USART2 Rx */

    USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

void uart_write_data(UART_Type uartType, uint8_t* data_ptr, uint16_t len)
{
    switch (uartType)
    {
        case UART_NUM1:
            DMA_SetCurrDataCounter(DMA1_Channel7, len);
            memcpy(txBuffer1, data_ptr, len);
            USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
            break;
        case UART_NUM2:
            DMA_SetCurrDataCounter(DMA1_Channel2, len);
            memcpy(txBuffer2, data_ptr, len);
            USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
            break;
        default:
            break;
    }
}
