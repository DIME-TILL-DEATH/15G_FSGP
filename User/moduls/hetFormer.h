#ifndef USER_MODULS_HETFORMER_H_
#define USER_MODULS_HETFORMER_H_

#include <stdint.h>

#include "debug.h"

#define NUM_WORDS16_FTW0 3
#define NUM_WORDS16_AMPLITUDE 1

#define HET_FTW0_REG_ADDRES_HW 0x01AB
#define HET_FTW0_REG_SIZE_16WORD 3

typedef enum
{
    FILTER_OFF = 0,
    FILTER_ON
}HET_FilterState_t;

typedef struct
{
    uint16_t FTW0_Ch1[NUM_WORDS16_FTW0];
    uint16_t FTW0_Ch2[NUM_WORDS16_FTW0];
    HET_FilterState_t filterState[6];
    uint16_t amplitudeCorrectionCh1;
    uint16_t amplitudeCorrectionCh2;
}HET_Data_t;

void HET_Init();
void HET_SetHeterodine(uint8_t freqNum);


#endif /* USER_MODULS_HETFORMER_H_ */
