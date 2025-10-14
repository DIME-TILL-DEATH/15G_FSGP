#ifndef USER_MODULS_LFMFORMER_H_
#define USER_MODULS_LFMFORMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "fsgp_command_frame.h"


typedef enum
{
    PS_OFF = 0,
    PS_SIN,
    PS_NOISE,
    PS_LFM
}PILOT_type_t;

typedef struct
{
    uint8_t impNum;
    uint16_t impLength;   // §¥§Ú§ã§Ü§â§Ö§ä§à§Ó §é§Ñ§ã§ä§à§ä§í
    uint16_t period;      // §¥§Ú§ã§Ü§â§Ö§ä§à§Ó §é§Ñ§ã§ä§à§ä§í   
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

typedef struct
{
    float fStart;
    float fStop;
}LfmBand_t;

extern LfmPack_t packData[];
extern LfmBand_t lfmBand[];

void LFM_Init();
void LFM_WriteStartupData();
void LFM_SetPack(DdsRegisterData_t* ddsData);
void LFM_SetPackBuffered(DdsRegisterData_t* ddsData);
//void LFM_RecalcImitData(double_t delay, double_t dopplerFreq);
DdsRegisterData_t LFM_CalcPackData(LfmPack_t pack, float fStart, float fStop, float delay, float dopplerFreq);
DdsRegisterData_t LFM_GetPackData(const FSGP_Command_Frame* dataFrame);

void LFM_SetStage2();

#endif /* USER_MODULS_LFMFORMER_H_ */
