#include "lfm_fifo.h"

uint16_t lfmBuf_wr_index, lfmBuf_rd_index, lfmBuf_counter;
DDS1508_Command_t lfmCommand_buf[LFM_FIFO_SIZE];

void LfmFIFO_Init()
{
    lfmBuf_wr_index = 0;
    lfmBuf_rd_index = 0;
    lfmBuf_counter = 0;
}

bool LfmFIFO_PutData(DDS1508_Command_t new_data)
{
    if(lfmBuf_counter < LFM_FIFO_SIZE)
    {
        lfmCommand_buf[lfmBuf_wr_index++] = new_data;

        if(lfmBuf_wr_index == LFM_FIFO_SIZE) lfmBuf_wr_index = 0;

        lfmBuf_counter++;
        return true;
    }
    else return false;
}

DDS1508_Command_t LfmFIFO_GetData()
{
    DDS1508_Command_t data;

    if(lfmBuf_counter == 0)
    {
        DDS1508_Command_t zero_command = {0};
        return zero_command;
    }

    data = lfmCommand_buf[lfmBuf_rd_index++];

    if(lfmBuf_rd_index == LFM_FIFO_SIZE) lfmBuf_rd_index = 0;

    lfmBuf_counter--;

    return data;
}

DDS1508_Command_t LfmFIFO_PeekData()
{
    return lfmCommand_buf[lfmBuf_rd_index];
}

uint8_t LfmFIFO_Count()
{
    return lfmBuf_counter;
}
