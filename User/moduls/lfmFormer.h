#ifndef USER_MODULS_LFMFORMER_H_
#define USER_MODULS_LFMFORMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "debug.h"
#include "dds1508.h"

#include "pack_data.h"

typedef struct
{
    uint16_t impNum;
    double impLength;   // §¥§Ú§ã§Ü§â§Ö§ä§à§Ó §é§Ñ§ã§ä§à§ä§í
    double period;      // §¥§Ú§ã§Ü§â§Ö§ä§à§Ó §é§Ñ§ã§ä§à§ä§í
    uint8_t sign;       // 0 - §á§à§Ý§à§Ø§Ú§ä§Ö§Ý§î§ß§í§Û §ß§Ñ§Ü§Ý§à§ß, 1-§à§ä§â§Ú§è§Ñ§ä§Ö§Ý§î§ß§í§Û
}LfmPack_t;

typedef enum
{
    IDLE = 0,
    ADR_SET,
    ADR_WR,
    DATA_SET,
    DATA_WR
}LfmSendState_t;

typedef struct
{
    uint16_t tph1[3];
    uint16_t tph2[3];
    uint16_t tph3[3];
    uint16_t tph4[3];

    uint16_t startF[3];
    uint16_t deltaF[3];
}DdsRegisterData_t;

void LFM_Init();
void LFM_WriteStartupData();
void LFM_SetPack(uint8_t packNumber);
void LFM_SetPackBuffered(uint8_t packNumber);
void LFM_RecalcImitData(double_t delay, double_t dopplerFreq);

void LFM_SetStage2();

#endif /* USER_MODULS_LFMFORMER_H_ */
