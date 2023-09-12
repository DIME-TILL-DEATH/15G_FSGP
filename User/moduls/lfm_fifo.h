#ifndef USER_MODULS_LFM_FIFO_H_
#define USER_MODULS_LFM_FIFO_H_

#include <stdint.h>
#include <stdbool.h>

#include "dds1508.h"

#define LFM_FIFO_SIZE 256

void LfmFIFO_Init();

bool LfmFIFO_PutData(DDS1508_Command_t new_data);
// get and permanently delete next data
DDS1508_Command_t LfmFIFO_GetData();
// view next data
DDS1508_Command_t LfmFIFO_PeekData();

uint8_t LfmFIFO_Count();

#endif /* USER_MODULS_LFM_FIFO_H_ */
