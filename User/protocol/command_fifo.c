#include "command_fifo.h"

uint8_t buf_wr_index, buf_rd_index, buf_counter;
FSGP_Command_Frame command_buf[COMMAND_FIFO_SIZE];
FSGP_Command_Frame zeroPack = {0};

void CommFIFO_Init()
{
    buf_wr_index = 0;
    buf_rd_index = 0;
    buf_counter = 0;

    zeroPack.NKCH = 3;
}

bool CommFIFO_PutData(FSGP_Command_Frame new_data)
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

FSGP_Command_Frame* CommFIFO_GetData()
{
    FSGP_Command_Frame* data;

    if(buf_counter == 0)
    {
        return &zeroPack;
    }

    data = &command_buf[buf_rd_index++];

    if(buf_rd_index == COMMAND_FIFO_SIZE) buf_rd_index = 0;

    buf_counter--;

    return data;
}

FSGP_Command_Frame CommFIFO_PeekData()
{
    return command_buf[buf_rd_index];
}

uint8_t CommFIFO_Count()
{
    return buf_counter;
}

void CommFIFO_Clear()
{
    buf_wr_index = 0;
    buf_rd_index = 0;
    buf_counter = 0;
}
