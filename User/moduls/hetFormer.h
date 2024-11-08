#ifndef USER_MODULS_HETFORMER_H_
#define USER_MODULS_HETFORMER_H_

#include <stdint.h>

#include "debug.h"

#define  FILTER_COUNT 8

#define HET_SERIAL_REG_ADDRES 0x0005

#define HET_FTW0_REG_SIZE 6
#define HET_FTW0_REG_ADDRES_HW 0x01AB

#define HET_FS_CURRENT_ADDRES_HW 0x040C
#define HET_FS_CURRENT_REG_SIZE 2

typedef enum
{
    FILTER_OFF = 0,
    FILTER_ON
}HET_FilterState_t;

typedef struct
{
    uint8_t FTW0_Ch1[HET_FTW0_REG_SIZE];
    uint8_t FTW0_Ch2[HET_FTW0_REG_SIZE];
    HET_FilterState_t filterState[FILTER_COUNT];
    uint8_t amplitudeCorrectionCh1[HET_FS_CURRENT_REG_SIZE];
    uint8_t amplitudeCorrectionCh2[HET_FS_CURRENT_REG_SIZE];
}HET_Data_t;

void HET_Init();
void HET_SetHeterodine(uint8_t freqNum);
void HET_UpdateIO();

void HET_SetFilters(uint8_t freqNum);

// temporally here
void setLatch(uint8_t val, uint8_t latchNum);

#endif /* USER_MODULS_HETFORMER_H_ */
