#include <math.h>
#include <memory.h>

#include "control_pin.h"

#include "spi_heterodine.h"

#include "het_data.h"
#include "hetFormer.h"

uint64_t calcFTW0fromDiv(uint16_t divider);
void sendFTW0(uint16_t* FTW0);
void sendSPIData(uint16_t* data, uint8_t numWords);

HET_Data_t hetData[FREQ_COUNT];

ControlPin_t pinFilter[FILTER_COUNT];

void HET_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    pinFilter[0].pin = GPIO_Pin_6;
    pinFilter[0].port = GPIOC;
    GPIO_InitStructure.GPIO_Pin = pinFilter[0].pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pinFilter[0].port, &GPIO_InitStructure);

    pinFilter[1].pin = GPIO_Pin_7;
    pinFilter[1].port = GPIOC;
    GPIO_InitStructure.GPIO_Pin = pinFilter[1].pin;
    GPIO_Init(pinFilter[0].port, &GPIO_InitStructure);

    for(uint8_t i=0; i < FREQ_COUNT; i++)
    {
        uint64_t ftw0_ch1 = calcFTW0fromDiv(dividerHet1[i]);
        hetData[i].FTW0_Ch1[0] = (ftw0_ch1>>40) & 0xFF;
        hetData[i].FTW0_Ch1[1] = (ftw0_ch1>>32) & 0xFF;
        hetData[i].FTW0_Ch1[2] = (ftw0_ch1>>24) & 0xFF;
        hetData[i].FTW0_Ch1[3] = (ftw0_ch1>>16) & 0xFF;
        hetData[i].FTW0_Ch1[4] = (ftw0_ch1>>8) & 0xFF;
        hetData[i].FTW0_Ch1[5] = (ftw0_ch1>>0) & 0xFF;

        uint64_t ftw0_ch2 = calcFTW0fromDiv(dividerHet2[i]);
        hetData[i].FTW0_Ch2[0] = (ftw0_ch2>>40) & 0xFF;
        hetData[i].FTW0_Ch2[1] = (ftw0_ch2>>32) & 0xFF;
        hetData[i].FTW0_Ch2[2] = (ftw0_ch2>>24) & 0xFF;
        hetData[i].FTW0_Ch2[3] = (ftw0_ch2>>16) & 0xFF;
        hetData[i].FTW0_Ch2[4] = (ftw0_ch2>>8) & 0xFF;
        hetData[i].FTW0_Ch2[5] = (ftw0_ch2>>0) & 0xFF;

        for(uint8_t j=0; j<FILTER_COUNT; j++)
        {
            hetData[i].filterState[j] = hetFilterStates[i][j];
        }
    }

    SPIHET_Init();
}

void HET_SetHeterodine(uint8_t freqNum)
{
    freqNum = freqNum - 1;
//    if(freqNum > FREQ_COUNT)
//    {
//       // printf("Invalid NKCH: %d\r\n", freqNum);
//        return;
//    }

    SPIHET_SendData_t dataToSend;

    dataToSend.channel = HET_CHANNEL1;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch1[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE;

    SPIHET_PutDataInSendBuffer(&dataToSend);

    dataToSend.channel = HET_CHANNEL2;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch2[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE;

    SPIHET_PutDataInSendBuffer(&dataToSend);
    SPIHET_ProcessSpiFifo();
}

uint8_t updateIOData = 0x01;

void HET_UpdateIO()
{
    SPIHET_SendData_t dataToSend;

    dataToSend.channel = HET_CHANNEL1;
    dataToSend.regAddress = HET_SERIAL_REG_ADDRES;
    dataToSend.data_ptr = &updateIOData;
    dataToSend.dataLen = 1;

    SPIHET_SendSpiData(&dataToSend); // send as fast as we can

    dataToSend.channel = HET_CHANNEL2;
    dataToSend.regAddress = HET_SERIAL_REG_ADDRES;
    dataToSend.data_ptr = &updateIOData;
    dataToSend.dataLen = 1;

    SPIHET_PutDataInSendBuffer(&dataToSend);
}

uint64_t calcFTW0fromDiv(uint16_t divider)
{
    return pow(2, 48) / divider;
}

void HET_SetFilters(uint8_t freqNum)
{
    for(uint8_t i=0; i<FILTER_COUNT; i++)
    {
        GPIO_WriteBit(pinFilter[i].port, pinFilter[i].pin, hetData[freqNum].filterState[i]);
    }
}
