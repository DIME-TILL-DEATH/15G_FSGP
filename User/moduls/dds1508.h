#ifndef USER_DDS1508_H_
#define USER_DDS1508_H_

#include "math.h"
#include "stdint.h"

#define DDS1508_ADDR_CH1_TPH1_L 0x1010
#define DDS1508_ADDR_CH1_TPH1_M 0x1011
#define DDS1508_ADDR_CH1_TPH1_H 0x1012

#define DDS1508_ADDR_CH1_TPH2_L 0x1014
#define DDS1508_ADDR_CH1_TPH2_M 0x1015
#define DDS1508_ADDR_CH1_TPH2_H 0x1016

#define DDS1508_ADDR_CH1_TPH3_L 0x1018
#define DDS1508_ADDR_CH1_TPH3_M 0x1019
#define DDS1508_ADDR_CH1_TPH3_H 0x101A

#define DDS1508_ADDR_CH1_TPH4_L 0x101C
#define DDS1508_ADDR_CH1_TPH4_M 0x101D
#define DDS1508_ADDR_CH1_TPH4_H 0x101E

#define DDS1508_ADDR_CH1_F_L 0x1020
#define DDS1508_ADDR_CH1_F_M 0x1021
#define DDS1508_ADDR_CH1_F_H 0x1022

#define DDS1508_ADDR_CH1_dF_L 0x1040
#define DDS1508_ADDR_CH1_dF_M 0x1041
#define DDS1508_ADDR_CH1_dF_H 0x1042

typedef struct
{
    uint16_t address;
    uint16_t value;
}DDS1508_Command_t;

void DDS1508_SetDiscretisationFreq(double_t sfreqDiscret);

uint64_t DDS1508_CalcFreqWord(double_t freqTarget);
uint64_t DDS1508_CalcDFWord(double_t freqStart, double_t freqStop, double_t lfmLength);

uint64_t DDS1508_CalcTWord(double_t lentgh);

#endif /* USER_DDS1508_H_ */
