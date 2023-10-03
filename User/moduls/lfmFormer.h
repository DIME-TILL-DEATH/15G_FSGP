#ifndef USER_MODULS_LFMFORMER_H_
#define USER_MODULS_LFMFORMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "debug.h"
#include "dds1508.h"

typedef struct
{
    uint16_t impNum;
    double impLength;   // ���ڧ�ܧ�֧��� ��ѧ�����
    double period;      // ���ڧ�ܧ�֧��� ��ѧ�����
    uint8_t sign;       // 0 - ���ݧ�اڧ�֧ݧ�ߧ�� �ߧѧܧݧ��, 1-����ڧ�ѧ�֧ݧ�ߧ��
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
    GPIO_TypeDef* port;
    uint16_t pin;
}ControlPin_t;

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
void LFM_SetPack(uint8_t packNumber);
void LFM_SetPackBuffered(uint8_t packNumber);
void LFM_RecalcImitData(bool enableImit, double_t delay, double_t dopplerFreq);

#endif /* USER_MODULS_LFMFORMER_H_ */
