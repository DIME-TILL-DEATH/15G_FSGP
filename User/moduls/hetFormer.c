#include <math.h>
#include <memory.h>

#include "spi_heterodine.h"

#include "het_data.h"
#include "hetFormer.h"

uint64_t calcFTW0fromDiv(uint16_t divider);
void sendFTW0(uint16_t* FTW0);
void sendSPIData(uint16_t* data, uint8_t numWords);

HET_Data_t hetData[FREQ_COUNT];

void HET_Init()
{
    for(uint8_t i=0; i < FREQ_COUNT; i++)
    {
        uint64_t ftw0_ch1 = calcFTW0fromDiv(dividerHet1[i]);
        hetData[i].FTW0_Ch1[0] = (ftw0_ch1>>32) & 0xFFFF;
        hetData[i].FTW0_Ch1[1] = (ftw0_ch1>>16) & 0xFFFF;
        hetData[i].FTW0_Ch1[2] = (ftw0_ch1>>0) & 0xFFFF;

        uint64_t ftw0_ch2 = calcFTW0fromDiv(dividerHet2[i]);
        hetData[i].FTW0_Ch2[0] = (ftw0_ch2>>32) & 0xFFFF;
        hetData[i].FTW0_Ch2[1] = (ftw0_ch2>>16) & 0xFFFF;
        hetData[i].FTW0_Ch2[2] = (ftw0_ch2>>0) & 0xFFFF;
    }

    SPIHET_Init();
}

void HET_SetHeterodine(uint8_t freqNum)
{
    freqNum = freqNum - 1;
    if(freqNum > FREQ_COUNT)
    {
        printf("Invalid NKCH\r\n");
        return;
    }

    SPIHET_SendData_t dataToSend;

    dataToSend.channel = HET_CHANNEL1;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch1[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE_16WORD;

    SPIHET_PutDataInSendBuffer(&dataToSend);

    dataToSend.channel = HET_CHANNEL2;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch2[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE_16WORD;

    SPIHET_PutDataInSendBuffer(&dataToSend);

    SPIHET_ProcessSpiData();
}

uint64_t calcFTW0fromDiv(uint16_t divider)
{
    return pow(2, 48) / divider;
}


