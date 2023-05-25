/*
 * command_fifo.c
 *
 *  Created on: May 24, 2023
 *      Author: Dmitriy Kostyuchik
 */

#include "command_fifo.h"

uint8_t buf_wr_index, buf_rd_index, buf_counter;
Command_Frame command_buf[32];

void CommFIFO_Init()
{
    buf_wr_index = 0;
    buf_rd_index = 0;
    buf_counter = 0;
}

bool CommFIFO_PutData(Command_Frame new_data)
{
    if(buf_counter < COMMAND_FIFO_SIZE)
    {
        command_buf[buf_wr_index++] = new_data;

        if(buf_wr_index == COMMAND_FIFO_SIZE) buf_wr_index = 0;

        buf_counter++;
        return true;
    }
    else return false;
}

Command_Frame CommFIFO_GetData()
{
    Command_Frame data;

    if(buf_counter == 0)
    {
        //doevents;
        // §Ó§à§Ù§Ó§â§Ñ§ë§Ñ§ä§î §ß§å§Ý§Ö§Ó§å§ð §á§Ñ§é§Ü§å §Ú§Ý§Ú §Ø§Õ§Ñ§ä§î §á§à§Ü§Ñ §Ò§å§æ§æ§Ö§â §ß§Ñ§á§à§Ý§ß§Ú§ä§ã§ñ?
        Command_Frame zero_pack = {0};
        return zero_pack;
    }

    data = command_buf[buf_rd_index++];

    if(buf_rd_index == COMMAND_FIFO_SIZE) buf_rd_index = 0;

    buf_counter--;

    return data;
}

Command_Frame CommFIFO_PeekData()
{
    return command_buf[buf_rd_index];
}

uint8_t CommFIFO_Count()
{
    return buf_counter;
}
