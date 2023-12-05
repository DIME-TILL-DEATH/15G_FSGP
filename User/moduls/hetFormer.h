#ifndef USER_MODULS_HETFORMER_H_
#define USER_MODULS_HETFORMER_H_

#include <stdint.h>

#include "debug.h"

#define HET_SERIAL_REG_ADDRES 0x0005

#define HET_FTW0_REG_SIZE 6
#define HET_FTW0_REG_ADDRES_HW 0x01AB

#define HET_AMPLITUDE_REG_SIZE 2

typedef enum
{
    FILTER_OFF = 0,
    FILTER_ON
}HET_FilterState_t;

typedef struct
{
    uint8_t FTW0_Ch1[HET_FTW0_REG_SIZE];
    uint8_t FTW0_Ch2[HET_FTW0_REG_SIZE];
    HET_FilterState_t filterState[6];
    uint8_t amplitudeCorrectionCh1[HET_AMPLITUDE_REG_SIZE];
    uint8_t amplitudeCorrectionCh2[HET_AMPLITUDE_REG_SIZE];
}HET_Data_t;

void HET_Init();
void HET_SetHeterodine(uint8_t freqNum);
void HET_UpdateIO();


#endif /* USER_MODULS_HETFORMER_H_ */
