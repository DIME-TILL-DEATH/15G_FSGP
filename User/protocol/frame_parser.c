#include <string.h>

#include "frame_parser.h"
#include "command_fifo.h"

#include "fsgp_command_frame.h"
#include "fsgp_fdk_frame.h"

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
//
//        outData[HEAD_AUX_OFFSET128_LW] = FSGP_ACKFRAME_AUX_DATA_POS128;
//        outData[HEAD_AUX_SIZE128_LW] = FSGP_ACKFRAME_AUX_DATA_SIZE128;
//
//        outData[HEADER_FRAME_TYPE_POS] = FSGP_ACK_FRAME;
//        outData[BODY_FRAME_TYPE_POS] = FSGP_ACK_FRAME;

        FrameHeader frameHeader;
        memset(frameHeader.rawData, 0, FRAME_HEADER_SIZE);

        frameHeader.structData.signature = __builtin_bswap16(FRAME_SIGNATURE);
        frameHeader.structData.RTK = 0;
        frameHeader.structData.TK = FSGP_ACK_FRAME;
        frameHeader.structData.RK = __builtin_bswap32(*outDataLen);
        frameHeader.structData.RF128 = __builtin_bswap16(*outDataLen/16);
        frameHeader.structData.PF = 1;
        frameHeader.structData.SCH = framesCounter;
        frameHeader.structData.NF = 0;

        Datagram_Header datagramHeader;
        memset(datagramHeader.rawData, 0, DATAGRAM_HEADER_SIZE);

        datagramHeader.structData.LAYOUT = 4;
        datagramHeader.structData.LAYOUT_SIZE128 = 3;
        datagramHeader.structData.RTK = 0;
        datagramHeader.structData.TK = FSGP_ACK_FRAME;
        datagramHeader.structData.RK = __builtin_bswap32(*outDataLen - FRAME_HEADER_SIZE);

        datagramHeader.structData.CTRL_OFFSET128 = __builtin_bswap32(FSGP_ACKFRAME_CTRL_OFFSET128);
        datagramHeader.structData.CTRL_SIZE128 = __builtin_bswap32(FSGP_ACKFRAME_CTRL_SIZE128);
        datagramHeader.structData.SYNC_OFFSET128 = __builtin_bswap32(FSGP_ACKFRAME_SYNC_OFFSET128);
        datagramHeader.structData.SYNC_SIZE128 = __builtin_bswap32(FSGP_ACKFRAME_SYNC_SIZE128);;
        datagramHeader.structData.HEAD_AUX_OFFSET128 = __builtin_bswap32(FSGP_ACKFRAME_AUX_DATA_OFFSET128);
        datagramHeader.structData.HEAD_AUX_SIZE128 = __builtin_bswap32(FSGP_ACKFRAME_AUX_DATA_SIZE128);
        datagramHeader.structData.SIGNAL_OFFSET128 = __builtin_bswap32(FSGP_ACKFRAME_SIGNAL_OFFSET128);
        datagramHeader.structData.SIGNAL_SIZE128 =  __builtin_bswap32(FSGP_ACKFRAME_SIGNAL_SIZE128);

        memcpy(&outData[0], frameHeader.rawData, FRAME_HEADER_SIZE);
        memcpy(&outData[FRAME_HEADER_SIZE], datagramHeader.rawData, DATAGRAM_HEADER_SIZE);

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
    if(!data_ptr) return;

    uint32_t datagramSize = FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE + FSGP_FDKFRAME_SIZE;

    FrameHeader frameHeader;
    memset(frameHeader.rawData, 0, FRAME_HEADER_SIZE);

    frameHeader.structData.signature = __builtin_bswap16(FRAME_SIGNATURE);
    frameHeader.structData.RTK = 0;
    frameHeader.structData.TK = FSGP_FDK_FRAME;
    frameHeader.structData.RK = __builtin_bswap32(datagramSize);
    frameHeader.structData.RF128 = __builtin_bswap16(datagramSize/16);
    frameHeader.structData.PF = 1;
    frameHeader.structData.SCH = framesCounter;
    frameHeader.structData.NF = 0;

    Datagram_Header datagramHeader;
    memset(datagramHeader.rawData, 0, DATAGRAM_HEADER_SIZE);

    datagramHeader.structData.LAYOUT = 4;
    datagramHeader.structData.LAYOUT_SIZE128 = 3;
    datagramHeader.structData.RTK = 0;
    datagramHeader.structData.TK = FSGP_FDK_FRAME;
    datagramHeader.structData.RK = __builtin_bswap32(datagramSize - FRAME_HEADER_SIZE);

    datagramHeader.structData.CTRL_OFFSET128 = __builtin_bswap32(FSGP_FDKFRAME_CTRL_OFFSET128);
    datagramHeader.structData.CTRL_SIZE128 = __builtin_bswap32(FSGP_FDKFRAME_CTRL_SIZE128);

    uint32_t othersOffsetValues = __builtin_bswap32(FSGP_FDKFRAME_CTRL_OFFSET128 + FSGP_FDKFRAME_CTRL_SIZE128);
    datagramHeader.structData.SYNC_OFFSET128 = othersOffsetValues;
    datagramHeader.structData.SYNC_SIZE128 = 0;
    datagramHeader.structData.HEAD_AUX_OFFSET128 = othersOffsetValues;
    datagramHeader.structData.HEAD_AUX_SIZE128 = 0;
    datagramHeader.structData.SIGNAL_OFFSET128 = othersOffsetValues;
    datagramHeader.structData.SIGNAL_SIZE128 = 0;

    FSGP_FDK_Frame fdkFrame;
    memset(fdkFrame.rawData, 0, FSGP_FDKFRAME_SIZE);

    // FDK data
    fdkFrame.structData.OSFSGP = 1;
    fdkFrame.structData.OTSSFSGP = 1;
    fdkFrame.structData.PGFSGP = 1;

    memcpy(&data_ptr[0], frameHeader.rawData, FRAME_HEADER_SIZE);
    memcpy(&data_ptr[FRAME_HEADER_SIZE], datagramHeader.rawData, DATAGRAM_HEADER_SIZE);
    memcpy(&data_ptr[FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE], fdkFrame.rawData, FSGP_FDKFRAME_SIZE);

    if(dataLen_ptr) *dataLen_ptr = datagramSize;
}
