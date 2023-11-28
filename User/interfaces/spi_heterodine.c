#include <memory.h>
#include <stdlib.h>

#include "spi_heterodine.h"

#define SPI_COMM_BUFFER_SIZE 16

SPIHET_SendData_t dataBuffer[SPI_COMM_BUFFER_SIZE];
SPIHET_SendData_t zeroData = {0};

#define SPI_BUFFER_SIZE 256
uint16_t spiBuffer[SPI_BUFFER_SIZE];

ControlPin_t PIN_CSCh1;
ControlPin_t PIN_CSCh2;

uint8_t fifo_wr_index, fifo_rd_index, fifo_counter;

void FIFO_Init()
{
    fifo_wr_index = 0;
    fifo_rd_index = 0;
    fifo_counter = 0;
}

void SPIHET_PutDataInSendBuffer(const SPIHET_SendData_t* data)
{
//    if(fifo_counter < SPI_COMM_BUFFER_SIZE)
//    {
        dataBuffer[fifo_wr_index++] = *data;

        if(fifo_wr_index == SPI_COMM_BUFFER_SIZE) fifo_wr_index = 0;

        fifo_counter++;
//        return true;
//    }
//    else return false;
}

SPIHET_SendData_t* FIFO_GetData()
{
    SPIHET_SendData_t* data;

    if(fifo_counter == 0)
    {
        return &zeroData;
    }

    data = &dataBuffer[fifo_rd_index++];

    if(fifo_rd_index == SPI_COMM_BUFFER_SIZE) fifo_rd_index = 0;

    fifo_counter--;

    return data;
}

uint8_t SPIHET_CommCount()
{
    return fifo_counter;
}
//====================================================================
void SPI_DMA_Init()
{
    DMA_InitTypeDef DMA_InitStructure={0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Channel2);

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spiBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel2, &DMA_InitStructure);

    DMA_ITConfig(DMA2_Channel2, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA2_Channel2, ENABLE); /* SPI3 Tx */

    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
}
//====================================================================
void SPIHET_Init()
{
    FIFO_Init();

    GPIO_InitTypeDef GPIO_InitStructure={0};
    SPI_InitTypeDef SPI_InitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);

    PIN_CSCh1.pin = GPIO_Pin_6;
    PIN_CSCh1.port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = PIN_CSCh1.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PIN_CSCh1.port, &GPIO_InitStructure);

    PIN_CSCh2.pin = GPIO_Pin_7;
    PIN_CSCh2.port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = PIN_CSCh2.pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PIN_CSCh2.port, &GPIO_InitStructure);

    GPIO_SetBits(PIN_CSCh1.port, PIN_CSCh1.pin);
    GPIO_SetBits(PIN_CSCh2.port, PIN_CSCh2.pin);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI3, &SPI_InitStructure);

    SPI_Cmd(SPI3, ENABLE);

    SPI_DMA_Init();
}

void SPIHET_ProcessSpiData()
{
    if(SPIHET_CommCount() > 0)
    {
        SPIHET_SendData_t* nextData_ptr = FIFO_GetData();

        switch(nextData_ptr->channel)
        {
            case HET_CHANNEL1:
            {
                GPIO_ResetBits(PIN_CSCh1.port, PIN_CSCh1.pin);
                break;
            }
            case HET_CHANNEL2:
            {
                GPIO_ResetBits(PIN_CSCh2.port, PIN_CSCh2.pin);
                break;
            }
            default: return;
        }

        uint16_t command = 0x6000 | (nextData_ptr->regAddress & 0x1FFF); // 0x6 = 0110 - Write + streaming mode

        if(nextData_ptr->data_ptr !=0 && nextData_ptr->dataLen != 0)
        {
            memcpy(&spiBuffer[0], &command, 2);
            memcpy(&spiBuffer[1], nextData_ptr->data_ptr, nextData_ptr->dataLen * 2);
            DMA_SetCurrDataCounter(DMA2_Channel2, nextData_ptr->dataLen + 1);
        }
    }
}


void SPIHET_Task()
{
    if(SPI_I2S_GetFlagStatus( SPI3, SPI_I2S_FLAG_BSY ) == RESET)
    {
        GPIO_SetBits(PIN_CSCh1.port, PIN_CSCh1.pin);
        GPIO_SetBits(PIN_CSCh2.port, PIN_CSCh2.pin);
        SPIHET_ProcessSpiData();
    }
}
