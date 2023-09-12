#include "lfmFormer.h"

#include "lfm_fifo.h"
#include "pack_data.h"

#define STAGE1_LENGTH 0
#define STAGE2_LENGTH 2

LfmPack_t packData[PACK_COUNT+1] =
{
        // pack 0 - zero pack
        {
            .sign = 0,
            .impLength = 10,
            .period = 100,
            .impNum = 2
        },
        {
             .sign = PACK1_SIGN,
             .impLength = PACK1_IMP_LENGTH,
             .period = PACK1_PERIOD,
             .impNum = PACK1_IMP_NUM
        },
        {
             .sign = PACK2_SIGN,
             .impLength = PACK2_IMP_LENGTH,
             .period = PACK2_PERIOD,
             .impNum = PACK2_IMP_NUM
        },
        {
             .sign = PACK3_SIGN,
             .impLength = PACK3_IMP_LENGTH,
             .period = PACK3_PERIOD,
             .impNum = PACK3_IMP_NUM
        },
        {
             .sign = PACK4_SIGN,
             .impLength = PACK4_IMP_LENGTH,
             .period = PACK4_PERIOD,
             .impNum = PACK4_IMP_NUM
        },
        {
             .sign = PACK5_SIGN,
             .impLength = PACK5_IMP_LENGTH,
             .period = PACK5_PERIOD,
             .impNum = PACK5_IMP_NUM
        },
        {
             .sign = PACK6_SIGN,
             .impLength = PACK6_IMP_LENGTH,
             .period = PACK6_PERIOD,
             .impNum = PACK6_IMP_NUM
        },
        {
             .sign = PACK7_SIGN,
             .impLength = PACK7_IMP_LENGTH,
             .period = PACK7_PERIOD,
             .impNum = PACK7_IMP_NUM
        },
        {
             .sign = PACK8_SIGN,
             .impLength = PACK8_IMP_LENGTH,
             .period = PACK8_PERIOD,
             .impNum = PACK8_IMP_NUM
        },
        {
             .sign = PACK9_SIGN,
             .impLength = PACK9_IMP_LENGTH,
             .period = PACK9_PERIOD,
             .impNum = PACK9_IMP_NUM
        },
        {
             .sign = PACK10_SIGN,
             .impLength = PACK10_IMP_LENGTH,
             .period = PACK10_PERIOD,
             .impNum = PACK10_IMP_NUM
        },
        {
             .sign = PACK11_SIGN,
             .impLength = PACK11_IMP_LENGTH,
             .period = PACK11_PERIOD,
             .impNum = PACK11_IMP_NUM
        },
        {
             .sign = PACK12_SIGN,
             .impLength = PACK12_IMP_LENGTH,
             .period = PACK12_PERIOD,
             .impNum = PACK12_IMP_NUM
        },
        {
             .sign = PACK13_SIGN,
             .impLength = PACK13_IMP_LENGTH,
             .period = PACK13_PERIOD,
             .impNum = PACK13_IMP_NUM
        },
        {
             .sign = PACK14_SIGN,
             .impLength = PACK14_IMP_LENGTH,
             .period = PACK14_PERIOD,
             .impNum = PACK14_IMP_NUM
        },
        {
             .sign = PACK15_SIGN,
             .impLength = PACK15_IMP_LENGTH,
             .period = PACK15_PERIOD,
             .impNum = PACK15_IMP_NUM
        }
};

DdsRegisterData_t ddsPackData[PACK_COUNT];

void TIM6_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));

DdsRegisterData_t calcPackData(LfmPack_t pack)
{
    DdsRegisterData_t outputData = {0};

    uint64_t freqStart;

    uint64_t dF;
    if(pack.sign)
    {
        freqStart = DDS1508_CalcFreqWord(FSTOP);
        dF = DDS1508_CalcDFWord(FSTOP, FSTART, pack.impLength);
    }
    else
    {
        freqStart = DDS1508_CalcFreqWord(FSTART);
        dF = DDS1508_CalcDFWord(FSTART, FSTOP, pack.impLength);
    }

    outputData.startF[0] = (freqStart & 0xFFFF);
    outputData.startF[1] = (freqStart & 0xFFFF0000) >> 16;
    outputData.startF[2] = (freqStart & 0xFFFF00000000) >> 32;

    outputData.deltaF[0] = (dF & 0xFFFF);
    outputData.deltaF[1] = (dF & 0xFFFF0000) >> 16;
    outputData.deltaF[2] = (dF & 0xFFFF00000000) >> 32;

    uint64_t tph3 = DDS1508_CalcTWord(pack.impLength);
    outputData.tph3[0] = (tph3 & 0xFFFF);
    outputData.tph3[1] = (tph3 & 0xFFFF0000) >> 16;
    outputData.tph3[2] = (tph3 & 0xFFFF00000000) >> 32;

    double_t lengthStage4 = pack.period - pack.impLength - STAGE1_LENGTH - STAGE2_LENGTH;
    uint64_t tph4 = DDS1508_CalcTWord(lengthStage4);
    outputData.tph4[0] = (tph4 & 0xFFFF);
    outputData.tph4[1] = (tph4 & 0xFFFF0000) >> 16;
    outputData.tph4[2] = (tph4 & 0xFFFF00000000) >> 32;

    return outputData;
}

void LFM_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    // Data[0:15]
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    DDS1508_SetDiscretisationFreq(FDISCRET);

    // fill built-in packs data on startup
    for(uint16_t i=0; i<PACK_COUNT; i++)
    {
        ddsPackData[i] = calcPackData(packData[PACK_COUNT]);
    }

    //  Timer for seting data intervals
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock;
    TIM_TimeBaseStructure.TIM_Prescaler = 36;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM6, ENABLE);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    NVIC_EnableIRQ(TIM6_IRQn);
}

void LFM_writeReg(uint16_t regAddress, uint16_t regData)
{

}

DDS1508_Command_t actualComm;
void TIM6_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}

