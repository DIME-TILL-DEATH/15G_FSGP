#include <string.h>

#include "frame_parser.h"
#include "command_fifo.h"

void parse_frame(uint8_t* inData, uint32_t inDataLen, uint8_t* outData, uint32_t* outDataLen)
{
    if(inData[COMMAND_FRAME_POS] == COMMAND_FRAME)
    {
        memcpy(outData, inData, inDataLen);

        Command_Frame *comand_ptr = (Command_Frame *)&(inData[COMMAND_DATA_POS]);
        Command_Frame recieved_command = *comand_ptr;

        convertEndians(&recieved_command);

        if(comm_fifo_putdata(recieved_command))
        {
            outData[BUFFER_SIZE_LW_POS] = COMMAND_FIFO_SIZE;
            outData[QUEUE_SIZE_LW_POS] = comm_fifo_count();
        }

        *outDataLen = 32*4;

        printf("Timestamp_lw: %X Index: %d TVRS: %d Command buffer: %d\r\n", recieved_command.timestamp_lw,
                                                                             recieved_command.index,
                                                                             recieved_command.TVRS,
                                                                             comm_fifo_count());
    }
}
