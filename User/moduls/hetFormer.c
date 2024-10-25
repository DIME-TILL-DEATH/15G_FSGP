#include <math.h>
#include <memory.h>

#include "control_pin.h"

#include "spi_heterodine.h"

#include "het_data.h"
#include "hetFormer.h"

uint64_t calcFTW0fromDiv(double_t divider);
void sendFTW0(uint16_t* FTW0);
void sendSPIData(uint16_t* data, uint8_t numWords);

HET_Data_t hetData[FREQ_COUNT];

GPIO_TypeDef* LATCH_PORT;
ControlPin_t pinFilter[FILTER_COUNT];
ControlPin_t pinLatchTr[4];

void HET_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // D-latch
    uint16_t pin_num = 0;
    LATCH_PORT = GPIOD;
    for(uint8_t i = 0; i<8; i++)
    {
        pinFilter[i].pin = pin_num;
        pinFilter[i].port = LATCH_PORT;
        GPIO_InitStructure.GPIO_Pin = pinFilter[i].pin;
        GPIO_Init(pinFilter[i].port, &GPIO_InitStructure);
        pin_num = pin_num<<1;
    }

    pinLatchTr[0].pin = GPIO_Pin_6;
    pinLatchTr[0].port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = pinLatchTr[0].pin;
    GPIO_Init(pinFilter[0].port, &GPIO_InitStructure);

    pinLatchTr[1].pin = GPIO_Pin_5;
    pinLatchTr[1].port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = pinLatchTr[1].pin;
    GPIO_Init(pinFilter[1].port, &GPIO_InitStructure);

    pinLatchTr[2].pin = GPIO_Pin_4;
    pinLatchTr[2].port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = pinLatchTr[2].pin;
    GPIO_Init(pinFilter[2].port, &GPIO_InitStructure);

    pinLatchTr[3].pin = GPIO_Pin_3;
    pinLatchTr[3].port = GPIOB;
    GPIO_InitStructure.GPIO_Pin = pinLatchTr[3].pin;
    GPIO_Init(pinFilter[3].port, &GPIO_InitStructure);

//    pinFilter[0].pin = GPIO_Pin_8;
//    pinFilter[0].port = GPIOC;
    pinFilter[0].pin = GPIO_Pin_0;
    pinFilter[0].port = GPIOD;
    GPIO_InitStructure.GPIO_Pin = pinFilter[0].pin;
    GPIO_Init(pinFilter[0].port, &GPIO_InitStructure);
//
//    pinFilter[1].pin = GPIO_Pin_9;
//    pinFilter[1].port = GPIOC;
    pinFilter[1].pin = GPIO_Pin_1;
    pinFilter[1].port = GPIOD;
    GPIO_InitStructure.GPIO_Pin = pinFilter[1].pin;
    GPIO_Init(pinFilter[1].port, &GPIO_InitStructure);
//
//    pinFilter[2].pin = GPIO_Pin_2;
//    pinFilter[2].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[2].pin;
//    GPIO_Init(pinFilter[2].port, &GPIO_InitStructure);
//
//    pinFilter[3].pin = GPIO_Pin_3;
//    pinFilter[3].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[3].pin;
//    GPIO_Init(pinFilter[3].port, &GPIO_InitStructure);
//
//    pinFilter[4].pin = GPIO_Pin_4;
//    pinFilter[4].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[4].pin;
//    GPIO_Init(pinFilter[4].port, &GPIO_InitStructure);
//
//    pinFilter[5].pin = GPIO_Pin_5;
//    pinFilter[5].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[5].pin;
//    GPIO_Init(pinFilter[5].port, &GPIO_InitStructure);
//
//    pinFilter[6].pin = GPIO_Pin_6;
//    pinFilter[6].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[6].pin;
//    GPIO_Init(pinFilter[6].port, &GPIO_InitStructure);
//
//    pinFilter[7].pin = GPIO_Pin_7;
//    pinFilter[7].port = GPIOD;
//    GPIO_InitStructure.GPIO_Pin = pinFilter[7].pin;
//    GPIO_Init(pinFilter[7].port, &GPIO_InitStructure);

    for(uint8_t i=0; i < FREQ_COUNT; i++)
    {
//        uint64_t ftw0_ch1 = calcFTW0fromDiv(dividerHet1[i]);
        uint64_t ftw0_ch1 = fixWord1[i];
        hetData[i].FTW0_Ch1[0] = (ftw0_ch1>>40) & 0xFF;
        hetData[i].FTW0_Ch1[1] = (ftw0_ch1>>32) & 0xFF;
        hetData[i].FTW0_Ch1[2] = (ftw0_ch1>>24) & 0xFF;
        hetData[i].FTW0_Ch1[3] = (ftw0_ch1>>16) & 0xFF;
        hetData[i].FTW0_Ch1[4] = (ftw0_ch1>>8) & 0xFF;
        hetData[i].FTW0_Ch1[5] = (ftw0_ch1>>0) & 0xFF;

        printf("Het1 FTW0:%x%x%x%x%x%x\r\n",
                hetData[i].FTW0_Ch1[0], hetData[i].FTW0_Ch1[1], hetData[i].FTW0_Ch1[2],
                hetData[i].FTW0_Ch1[3], hetData[i].FTW0_Ch1[4], hetData[i].FTW0_Ch1[5]);

        amplWord1[i] = 0x0200;
        hetData[i].amplitudeCorrectionCh1[0] = (amplWord1[i]>>8) & 0xFF;
        hetData[i].amplitudeCorrectionCh1[1] = (amplWord1[i]>>0) & 0xFF;


//        uint64_t ftw0_ch2 = calcFTW0fromDiv(dividerHet2[i]);
        uint64_t ftw0_ch2 = fixWord2[i];
        hetData[i].FTW0_Ch2[0] = (ftw0_ch2>>40) & 0xFF;
        hetData[i].FTW0_Ch2[1] = (ftw0_ch2>>32) & 0xFF;
        hetData[i].FTW0_Ch2[2] = (ftw0_ch2>>24) & 0xFF;
        hetData[i].FTW0_Ch2[3] = (ftw0_ch2>>16) & 0xFF;
        hetData[i].FTW0_Ch2[4] = (ftw0_ch2>>8) & 0xFF;
        hetData[i].FTW0_Ch2[5] = (ftw0_ch2>>0) & 0xFF;

        printf("Het2 FTW0:%x%x%x%x%x%x\r\n\r\n",
                        hetData[i].FTW0_Ch2[0], hetData[i].FTW0_Ch2[1], hetData[i].FTW0_Ch2[2],
                        hetData[i].FTW0_Ch2[3], hetData[i].FTW0_Ch2[4], hetData[i].FTW0_Ch2[5]);

//        amplWord2[i] = 0x0200;
        hetData[i].amplitudeCorrectionCh2[0] = (amplWord2[i]>>8) & 0xFF;
        hetData[i].amplitudeCorrectionCh2[1] = (amplWord2[i]>>0) & 0xFF;

        for(uint8_t j=0; j<6; j++) //FILTER_COUNT; j++)
        {
            hetData[i].filterState[j] = hetFilterStates[i][j];
        }
    }

    SPIHET_Init();
}

void setLatch(uint8_t val, uint8_t latchNum)
{
    uint16_t resultPortVal = 0;
    resultPortVal = LATCH_PORT->OUTDR & 0xFF00;
    resultPortVal = resultPortVal | val;

    LATCH_PORT->OUTDR = resultPortVal;

    pinLatchTr[latchNum].port->BSHR = pinLatchTr[latchNum].pin;
    pinLatchTr[latchNum].port->BCR = pinLatchTr[latchNum].pin;
}

void HET_SetHeterodine(uint8_t freqNum)
{
    freqNum = freqNum - 1;
//    if(freqNum > FREQ_COUNT)
//    {
//       // printf("Invalid NKCH: %d\r\n", freqNum);
//        return;
//    }

    if(freqNum > FREQ_COUNT) return;

    SPIHET_SendData_t dataToSend;

    dataToSend.channel = HET_CHANNEL1;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch1[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE;
    SPIHET_PutDataInSendBuffer(&dataToSend);

    dataToSend.regAddress = HET_FS_CURRENT_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].amplitudeCorrectionCh1[0]);
    dataToSend.dataLen = HET_FS_CURRENT_REG_SIZE;
    SPIHET_PutDataInSendBuffer(&dataToSend);

    dataToSend.channel = HET_CHANNEL2;
    dataToSend.regAddress = HET_FTW0_REG_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].FTW0_Ch2[0]);
    dataToSend.dataLen = HET_FTW0_REG_SIZE;
    SPIHET_PutDataInSendBuffer(&dataToSend);

    dataToSend.regAddress = HET_FS_CURRENT_ADDRES_HW;
    dataToSend.data_ptr = &(hetData[freqNum].amplitudeCorrectionCh2[0]);
    dataToSend.dataLen = HET_FS_CURRENT_REG_SIZE;
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

uint64_t calcFTW0fromDiv(double_t divider)
{
    return pow(2, 48) / divider;
}

void HET_SetFilters(uint8_t freqNum)
{
    freqNum -=1;
    if(freqNum > FREQ_COUNT)
    {
        printf("Set filters: incorrect NKCH: %d", freqNum++);
        return;
    }

    for(uint8_t i=0; i<FILTER_COUNT; i++)
    {
        GPIO_WriteBit(pinFilter[i].port, pinFilter[i].pin, hetData[freqNum].filterState[i]);
    }
}
