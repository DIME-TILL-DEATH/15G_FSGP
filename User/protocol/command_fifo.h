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

void init_comm_fifo();

bool comm_fifo_putdata(Command_Frame new_data);
// get and permanently delete next data
Command_Frame comm_fifo_getdata();
// view next data
Command_Frame comm_fifo_peekdata();

unsigned char comm_fifo_count();

void convertEndians(Command_Frame* comm);

#endif /* USER_PROTOCOL_COMMAND_FIFO_H_ */
