#include <string.h>

#include "frame_parser.h"
#include "command_fifo.h"

void mConvertEndians(FSGP_Command_Frame* comm);

void parseFrame(const uint8_t* inData, uint32_t inDataLen, uint8_t* outData, uint32_t* outDataLen)
{
    *outDataLen = 0;

    if(inData[HEADER_FRAME_TYPE_POS] == FSGP_COMMAND_FRAME)
    {
        FSGP_Command_Frame *comand_ptr = (FSGP_Command_Frame *)&(inData[COMMAND_DATA_POS]);
        FSGP_Command_Frame recieved_command = *comand_ptr;

        mConvertEndians(&recieved_command);

        if(CommFIFO_PutData(recieved_command))
        {
            memcpy(outData, inData, inDataLen);

            outData[FSGP_BUFFER_SIZE_LW_POS] = COMMAND_FIFO_SIZE;
            outData[FSGP_QUEUE_SIZE_LW_POS] = CommFIFO_Count();

            *outDataLen = inDataLen + 16;
        }

        outData[HEAD_AUX_OFFSET128_LW] = FSGP_ACKFRAME_AUX_DATA_POS128;
        outData[HEAD_AUX_SIZE128_LW] = FSGP_ACKFRAME_DATA_SIZE128;

        outData[HEADER_FRAME_TYPE_POS] = FSGP_ACK_FRAME;
        outData[BODY_FRAME_TYPE_POS] = FSGP_ACK_FRAME;

//        printf("Timestamp_lw: %X Index: %d TVRS: %d Command buffer: %d\r\n", recieved_command.timestamp_lw,
//                                                                             recieved_command.index,
//                                                                             recieved_command.TVRS,
//                                                                             CommFIFO_Count());
    }
}

void mConvertEndians(FSGP_Command_Frame* comm)
{
    comm->TVRS = __builtin_bswap32(comm->TVRS);
    comm->index = __builtin_bswap32(comm->index);
}

void getFdkPayload(uint8_t* data_ptr, uint16_t* dataLen_ptr)
{
    if(data_ptr)
    {
        // dummy
        for(int i=0; i<132; i++)
            data_ptr[i] = i;
    }

    data_ptr[HEADER_FRAME_TYPE_POS] = FSGP_FDK_FRAME;

    if(dataLen_ptr) *dataLen_ptr = 32;
}
