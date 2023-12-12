#include <string.h>

#include "frame_parser.h"
#include "command_fifo.h"

#include "hetFormer.h"

#include "fsgp_command_frame.h"
#include "fsgp_fdk_frame.h"
#include "fsgp_signal_params_frame.h"

#include "lfmFormer.h"

void mConvertEndians(FSGP_Command_Frame* comm);

void parseFrame(const uint8_t* inData, uint32_t inDataLen, uint8_t* outData, uint32_t* outDataLen)
{
    *outDataLen = 0;

    switch(inData[HEADER_FRAME_TYPE_POS])
    {
        case FSGP_COMMAND_FRAME:
        {
            FSGP_Command_Frame *comand_ptr = (FSGP_Command_Frame *)&(inData[COMMAND_DATA_POS]);
            FSGP_Command_Frame recieved_command = *comand_ptr;

           // printf("recieved nk4:%d\r\n", recieved_command.NKCH);

            mConvertEndians(&recieved_command);

            if(recieved_command.SBR_OCH) CommFIFO_Clear();

            if(CommFIFO_Count() == 0)
            {
                HET_SetHeterodine(recieved_command.NKCH);
            }

            if(CommFIFO_PutData(recieved_command))
            {
                memcpy(outData, inData, inDataLen);

                outData[FSGP_BUFFER_SIZE_LW_POS] = COMMAND_FIFO_SIZE;
                outData[FSGP_QUEUE_SIZE_LW_POS] = CommFIFO_Count();

                *outDataLen = inDataLen + 16;
            }
            else
            {
                printf("Comm buffer full!\r\n");
                return;
            }

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

            DatagramHeader datagramHeader;
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

            break;
        }

        case FSGP_SIGNAL_PARAMS_FRAME:
        {

            FSGP_Signal_Params_Frame *paramsFrame_ptr = (FSGP_Signal_Params_Frame *)&(inData[COMMAND_DATA_POS]);

            int16_t speedMS = __builtin_bswap16(paramsFrame_ptr->speed);
            uint16_t delay = __builtin_bswap16(paramsFrame_ptr->delay);

            printf("recieved signal description frame, speed: %d, delay: %d\r\n", speedMS, delay);

            float_t fDoppler = 2 * speedMS * FSTART / 300000000;

            LFM_RecalcImitData(delay, fDoppler);

            memcpy(outData, inData, inDataLen);
            *outDataLen = inDataLen;

            outData[HEADER_FRAME_TYPE_POS] = FSGP_ACK_SIGNAL_PARAMS_FRAME;
            outData[FRAME_HEADER_SIZE + HEADER_FRAME_TYPE_POS] = FSGP_ACK_SIGNAL_PARAMS_FRAME;
            break;
        }

        default:
            {

                printf("unknown frame type: %d\r\n", inData[HEADER_FRAME_TYPE_POS]);
            }
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

    DatagramHeader datagramHeader;
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
