#include "lfmFormer.h"

#include "lfm_fifo.h"

#define STAGE1_LENGTH 0 * 24
#define STAGE2_LENGTH 4 * 24

GPIO_TypeDef* DATA_PORT;
ControlPin_t PIN_CS, PIN_ADR, PIN_WR, PIN_RD;

static inline void LFM_WriteReg(uint16_t address, uint16_t value);

LfmPack_t packData[PACK_COUNT+1] =
{
        // pack 0 - zero pack
        {
            .sign = 0,
            .impLength = 120,
            .period = 600,
            .impNum = 4
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

DdsRegisterData_t ddsPackData[PACK_COUNT] = {0}; // Base pack(Zond impulse)

void TIM6_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));

/********************************************************
 * delay - in discrets 24MHz
 * doppler - in MHz
 *******************************************************/
DdsRegisterData_t calcPackData(LfmPack_t pack, double_t delay, double_t dopplerFreq)
{
    DdsRegisterData_t outputData = {0};

    uint64_t freqStart;

    uint64_t dF;
    if(pack.sign)
    {
        freqStart = DDS1508_CalcFreqWord(FSTOP+dopplerFreq);
        dF = DDS1508_CalcDFWord(FSTOP, FSTART, pack.impLength);
    }
    else
    {
        freqStart = DDS1508_CalcFreqWord(FSTART+dopplerFreq);
        dF = DDS1508_CalcDFWord(FSTART, FSTOP, pack.impLength);
    }

    outputData.startF[0] = (freqStart & 0xFFFF);
    outputData.startF[1] = (freqStart & 0xFFFF0000) >> 16;
    outputData.startF[2] = (freqStart & 0xFFFF00000000) >> 32;

    outputData.deltaF[0] = ((uint64_t)dF & 0xFFFF);
    outputData.deltaF[1] = ((uint64_t)dF & 0xFFFF0000) >> 16;
    outputData.deltaF[2] = ((uint64_t)dF & 0xFFFF00000000) >> 32;

    double_t stage1_length = STAGE1_LENGTH + delay;
    uint64_t tph1 = DDS1508_CalcTWord(stage1_length);
    outputData.tph1[0] = (tph1 & 0xFFFF);
    outputData.tph1[1] = (tph1 & 0xFFFF0000) >> 16;
    outputData.tph1[2] = (tph1 & 0xFFFF00000000) >> 32;

    double_t stage2_length = STAGE2_LENGTH;
    uint64_t tph2 = DDS1508_CalcTWord(stage2_length);
    outputData.tph2[0] = (tph2 & 0xFFFF);
    outputData.tph2[1] = (tph2 & 0xFFFF0000) >> 16;
    outputData.tph2[2] = (tph2 & 0xFFFF00000000) >> 32;

    uint64_t tph3 = DDS1508_CalcTWord(pack.impLength);
    outputData.tph3[0] = (tph3 & 0xFFFF);
    outputData.tph3[1] = (tph3 & 0xFFFF0000) >> 16;
    outputData.tph3[2] = (tph3 & 0xFFFF00000000) >> 32;

    double_t lengthStage4 = pack.period - pack.impLength - stage1_length - stage2_length;
    uint64_t tph4 = DDS1508_CalcTWord(lengthStage4);
    outputData.tph4[0] = (tph4 & 0xFFFF);
    outputData.tph4[1] = (tph4 & 0xFFFF0000) >> 16;
    outputData.tph4[2] = (tph4 & 0xFFFF00000000) >> 32;

    return outputData;
}

void LFM_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    // Data[0:15]
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    DATA_PORT = GPIOE;

    // CS, ADR, WR, RD
    PIN_CS.port = PIN_ADR.port = PIN_WR.port = PIN_RD.port = GPIOD;
    PIN_CS.pin = GPIO_Pin_12;
    PIN_ADR.pin = GPIO_Pin_13;
    PIN_WR.pin = GPIO_Pin_14;
    PIN_RD.pin = GPIO_Pin_15;

    GPIO_InitStructure.GPIO_Pin = PIN_CS.pin | PIN_ADR.pin | PIN_WR.pin | PIN_RD.pin;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(PIN_CS.port, PIN_CS.pin);
    GPIO_SetBits(PIN_ADR.port, PIN_ADR.pin);
    GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
    GPIO_SetBits(PIN_RD.port, PIN_RD.pin);

    DDS1508_SetDiscretisationFreq(FDISCRET);

    LfmFIFO_Init();

    // fill built-in packs data on startup
    for(uint16_t i=0; i<PACK_COUNT; i++)
    {
        ddsPackData[i] = calcPackData(packData[i], 0, 0);
        //ddsPackData[PACK_COUNT + 1 + i] = calcPackData(packData[i], 120, 0.2);
    }

    LFM_WriteStartupData();
}

/*
void LFM_RecalcImitData(double_t delay, double_t dopplerFreq)
{
    for(uint16_t i=0; i<PACK_COUNT+1; i++)
    {
        ddsPackData[PACK_COUNT + 1 + i] = calcPackData(packData[i], delay, dopplerFreq);
    }
}
*/
// Fast routine
static inline void LFM_WriteReg(uint16_t address, uint16_t value)
{
    PIN_ADR.port->BCR = PIN_ADR.pin;
    PIN_WR.port->BCR = PIN_WR.pin;
    DATA_PORT->OUTDR =  address;
    PIN_WR.port->BSHR = PIN_WR.pin;
    PIN_ADR.port->BSHR = PIN_ADR.pin;

    PIN_WR.port->BCR = PIN_WR.pin;
    DATA_PORT->OUTDR = value;
    PIN_WR.port->BSHR = PIN_WR.pin;
}


void LFM_WriteStartupData()
{
    PIN_CS.port->BCR = PIN_CS.pin;

    LFM_WriteReg(DDS1508_ADDR_SWRST, 0x0078);
    LFM_WriteReg(DDS1508_ADDR_CTR, 0x1000);
    LFM_WriteReg(DDS1508_ADDR_SYNC, 0x4182);
    LFM_WriteReg(DDS1508_ADDR_ROUTE, 0x0000);

    LFM_WriteReg(DDS1508_ADDR_CH1_F_H, ddsPackData[0].startF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_M, ddsPackData[0].startF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_L, ddsPackData[0].startF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_dF_H, ddsPackData[0].deltaF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_M, ddsPackData[0].deltaF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_L, ddsPackData[0].deltaF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH1_L, ddsPackData[0].tph1[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH2_L, ddsPackData[0].tph2[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH3_L, ddsPackData[0].tph3[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH4_L, ddsPackData[0].tph4[0]);

    LFM_WriteReg(DDS1508_ADDR_CLR, 0x003F);
    LFM_WriteReg(DDS1508_ADDR_CH1_LS_CTR, 0xBC10);

    PIN_CS.port->BSHR = PIN_CS.pin;
}

void LFM_SetPack(uint8_t packNumber)
{
    if(packNumber > PACK_COUNT) return;

    PIN_CS.port->BCR = PIN_CS.pin;
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH1_L, ddsPackData[packNumber].tph1[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_F_H, ddsPackData[packNumber].startF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_M, ddsPackData[packNumber].startF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_L, ddsPackData[packNumber].startF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_dF_H, ddsPackData[packNumber].deltaF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_M, ddsPackData[packNumber].deltaF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_L, ddsPackData[packNumber].deltaF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH3_L, ddsPackData[packNumber].tph3[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH4_L, ddsPackData[packNumber].tph4[0]);

    PIN_CS.port->BSHR = PIN_CS.pin;
}
//------------------------------------------------------------------------------
// bufferred (not used)
//------------------------------------------------------------------------------

void LFM_SetPackBuffered(uint8_t packNumber)
{
    DDS1508_Command_t comm;

    comm.address = DDS1508_ADDR_CH1_F_H;
    comm.value = ddsPackData[packNumber].startF[2];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_F_M;
    comm.value = ddsPackData[packNumber].startF[1];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_F_L;
    comm.value = ddsPackData[packNumber].startF[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_H;
    comm.value = ddsPackData[packNumber].deltaF[2];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_M;
    comm.value = ddsPackData[packNumber].deltaF[1];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_L;
    comm.value = ddsPackData[packNumber].deltaF[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_TPH3_L;
    comm.value = ddsPackData[packNumber].tph3[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_TPH4_L;
    comm.value = ddsPackData[packNumber].tph4[0];
    LfmFIFO_PutData(comm);

    NVIC_EnableIRQ(TIM6_IRQn);
}

DDS1508_Command_t actualComm;
LfmSendState_t sendingState = IDLE;
void TIM6_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    switch(sendingState)
    {
    case IDLE:
        if(LfmFIFO_Count() > 0)
        {
            actualComm = LfmFIFO_GetData();
            GPIO_ResetBits(PIN_CS.port, PIN_CS.pin);
            sendingState = ADR_SET;
        }
        else
        {
            GPIO_SetBits(PIN_CS.port, PIN_CS.pin);
            NVIC_DisableIRQ(TIM6_IRQn);
        }
        break;

    case ADR_SET:
        GPIO_Write(DATA_PORT, actualComm.address);
        GPIO_ResetBits(PIN_ADR.port, PIN_ADR.pin);
        GPIO_ResetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = ADR_WR;
        break;

    case ADR_WR:
        GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
        GPIO_SetBits(PIN_ADR.port, PIN_ADR.pin);
        sendingState = DATA_SET;
        break;

    case DATA_SET:
        GPIO_Write(DATA_PORT, actualComm.value);
        GPIO_ResetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = DATA_WR;
        break;

    case DATA_WR:
        GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = IDLE;
        break;
    }
}
