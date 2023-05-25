/*
 * command_fifo.h
 *
 *  Created on: May 24, 2023
 *      Author: Dmitriy Kostyuchik
 */

#ifndef USER_PROTOCOL_COMMAND_FIFO_H_
#define USER_PROTOCOL_COMMAND_FIFO_H_

#include <stdbool.h>
#include "command_frame.h"

#define COMMAND_FIFO_SIZE 32

void CommFIFO_Init();

bool CommFIFO_PutData(Command_Frame new_data);
// get and permanently delete next data
Command_Frame CommFIFO_GetData();
// view next data
Command_Frame CommFIFO_PeekData();

unsigned char CommFIFO_Count();

#endif /* USER_PROTOCOL_COMMAND_FIFO_H_ */
