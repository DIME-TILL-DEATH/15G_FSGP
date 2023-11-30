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
    dataToSend.dataLen = HET_FTW0_REG_SIZE;

    SPIHET_PutDataInSendBuffer(&dataToSend);
    SPIHET_ProcessSpiData(); // Send first command while filling buffer

    dataToSend.channel = HET_CHANNEL2;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch2[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE;

    SPIHET_PutDataInSendBuffer(&dataToSend);


}

uint64_t calcFTW0fromDiv(uint16_t divider)
{
    return pow(2, 48) / divider;
}


