#ifndef USER_PROTOCOL_COMMAND_FIFO_H_
#define USER_PROTOCOL_COMMAND_FIFO_H_

#include <fsgp_command_frame.h>
#include <stdbool.h>

#define COMMAND_FIFO_SIZE 64

void CommFIFO_Init();

bool CommFIFO_PutData(FSGP_Command_Frame new_data);
// Fast get and permanently delete next data
FSGP_Command_Frame* CommFIFO_GetData();
// view next data
FSGP_Command_Frame CommFIFO_PeekData();

void CommFIFO_Clear();

uint8_t CommFIFO_Count();

#endif /* USER_PROTOCOL_COMMAND_FIFO_H_ */
