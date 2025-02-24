#ifndef USER_PROTOCOL_COMMAND_FIFO_H_
#define USER_PROTOCOL_COMMAND_FIFO_H_

#include <stdbool.h>

#include "fsgp_command_frame.h"
#include "lfmFormer.h"

#define COMMAND_FIFO_SIZE 64

typedef struct{
    FSGP_Command_Frame rcvdFrame;
    DdsRegisterData_t ddsData;
}FSGP_Command_Data;

void CommFIFO_Init();

bool CommFIFO_PutData(FSGP_Command_Data new_data);
// Fast get and permanently delete next data
FSGP_Command_Data* CommFIFO_GetData();
// view next data
FSGP_Command_Data CommFIFO_PeekData();

void CommFIFO_Clear();

uint8_t CommFIFO_Count();

#endif /* USER_PROTOCOL_COMMAND_FIFO_H_ */
