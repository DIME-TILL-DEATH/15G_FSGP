#include <string.h>

#include "frame_parser.h"
#include "command_fifo.h"

#include "hetFormer.h"

#include "fsgp_command_frame.h"
#include "fsgp_fdk_frame.h"
#include "fsgp_signal_params_frame.h"
#include "fsgp_ready_frame.h"

#include "lfmFormer.h"

#include "ethernet.h"

void mConvertEndians(FSGP_Command_Frame* comm);

uint16_t ackFramesCounter = 0;
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
            else
            {
                if(CommFIFO_LastData().rcvdFrame.TVRS == recieved_command.TVRS){
                    return;
                }
            }

            if(ackFramesCounter == 0xFFFF) ackFramesCounter = 0;
            else ackFramesCounter++;

            FSGP_Command_Data commData;
//            commData.ddsData = LFM_CalcPackData(packData[recieved_command.KP], recieved_command.NLCHM, 0, 0);
            commData.ddsData = LFM_GetPackData(recieved_command.KP, recieved_command.NLCHM);
            commData.rcvdFrame = recieved_command;

            if(CommFIFO_PutData(&commData))
            {
                memcpy(outData, inData, inDataLen);

                outData[FSGP_BUFFER_SIZE_LW_POS] = COMMAND_FIFO_SIZE;
                outData[FSGP_QUEUE_SIZE_LW_POS] = CommFIFO_Count();

                *outDataLen = inDataLen + 16;
            }
            else
            {
                //printf("Comm buffer full!\r\n");
                return;
            }

            FrameHeader frameHeader;
            memset(frameHeader.rawData, 0, FRAME_HEADER_SIZE);

            frameHeader.structData.signature = __builtin_bswap16(FRAME_SIGNATURE);
            frameHeader.structData.RTK = 0;
            frameHeader.structData.TK = FSGP_ACK_FRAME;
            frameHeader.structData.RK = __builtin_bswap32(*outDataLen - 16);
            frameHeader.structData.RF128_PF = __builtin_bswap16(*outDataLen/16 | (1<<15));
            frameHeader.structData.SCH = __builtin_bswap16(ackFramesCounter);
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

//            printf("recieved signal description frame, speed: %d, delay: %d\r\n", speedMS, delay);

            //float_t fDoppler = 2 * speedMS * FSTART / 300000000;
            //LFM_RecalcImitData(delay, fDoppler);

            memcpy(outData, inData, inDataLen);
            *outDataLen = inDataLen;

            outData[HEADER_FRAME_TYPE_POS] = FSGP_ACK_SIGNAL_PARAMS_FRAME;
            outData[FRAME_HEADER_SIZE + HEADER_FRAME_TYPE_POS] = FSGP_ACK_SIGNAL_PARAMS_FRAME;
            break;
        }

        default:
            {

//                printf("unknown frame type: %d\r\n", inData[HEADER_FRAME_TYPE_POS]);
            }
    }
}

void mConvertEndians(FSGP_Command_Frame* comm)
{
    comm->TVRS = __builtin_bswap32(comm->TVRS);
    comm->index = __builtin_bswap32(comm->index);
}

uint16_t fdkFramesCounter = 0;
void getFdkFramePayload(uint8_t* data_ptr, uint16_t* dataLen_ptr)
{
    if(!data_ptr) return;

    if(fdkFramesCounter == 0xFFFF) fdkFramesCounter = 0;
    else fdkFramesCounter++;

    uint32_t datagramSize = FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE + FSGP_FDKFRAME_SIZE;

    FrameHeader frameHeader;
    memset(frameHeader.rawData, 0, FRAME_HEADER_SIZE);

    frameHeader.structData.signature = __builtin_bswap16(FRAME_SIGNATURE);
    frameHeader.structData.RTK = 0;
    frameHeader.structData.TK = FSGP_FDK_FRAME;
    frameHeader.structData.RK = __builtin_bswap32(datagramSize - 16);
    frameHeader.structData.RF128_PF = __builtin_bswap16(datagramSize/16 | (1<<15));
    frameHeader.structData.SCH = __builtin_bswap16(fdkFramesCounter);
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

    datagramHeader.structData.SYNC_OFFSET128 = __builtin_bswap32(FSGP_FDKFRAME_SYNC_OFFSET);
    datagramHeader.structData.SYNC_SIZE128 = __builtin_bswap32(FSGP_FDKFRAME_SYNC_SIZE);
    datagramHeader.structData.HEAD_AUX_OFFSET128 = __builtin_bswap32(FSGP_FDKFRAME_AUX_OFFSET);
    datagramHeader.structData.HEAD_AUX_SIZE128 = __builtin_bswap32(FSGP_FDKFRAME_AUX_SIZE);
    datagramHeader.structData.SIGNAL_OFFSET128 = __builtin_bswap32(FSGP_FDKFRAME_SIGNAL_OFFSET);
    datagramHeader.structData.SIGNAL_SIZE128 = __builtin_bswap32(FSGP_FDKFRAME_SIGNAL_SIZE);

    FSGP_FDK_Frame fdkFrame;
    memset(fdkFrame.rawData, 0, FSGP_FDKFRAME_SIZE);

    memcpy(&data_ptr[0], frameHeader.rawData, FRAME_HEADER_SIZE);
    memcpy(&data_ptr[FRAME_HEADER_SIZE], datagramHeader.rawData, DATAGRAM_HEADER_SIZE);

    // FDK data
    memcpy(&data_ptr[FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE], &(actualComm->rcvdFrame), sizeof(FSGP_Command_Frame));

    if(dataLen_ptr) *dataLen_ptr = datagramSize;
}

uint16_t readyFramesCounter = 0;
void getRdyFramePayload(uint8_t* data_ptr, uint16_t* dataLen_ptr)
{
    if(!data_ptr) return;

    if(readyFramesCounter == 0xFFFF) readyFramesCounter = 0;
    else readyFramesCounter++;

    uint32_t datagramSize = FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE + FSGP_READYFRAME_SIZE;

    FrameHeader frameHeader;
    memset(frameHeader.rawData, 0, FRAME_HEADER_SIZE);

    frameHeader.structData.signature = __builtin_bswap16(FRAME_SIGNATURE);
    frameHeader.structData.RTK = 0;
    frameHeader.structData.TK = FSGP_READY_FRAME;
    frameHeader.structData.RK = __builtin_bswap32(datagramSize - 16);
    frameHeader.structData.RF128_PF = __builtin_bswap16(datagramSize/16 | (1<<15));
    frameHeader.structData.SCH = __builtin_bswap16(readyFramesCounter);
    frameHeader.structData.NF = 0;

    DatagramHeader datagramHeader;
    memset(datagramHeader.rawData, 0, DATAGRAM_HEADER_SIZE);

    datagramHeader.structData.LAYOUT = 4;
    datagramHeader.structData.LAYOUT_SIZE128 = 3;
    datagramHeader.structData.RTK = 0;
    datagramHeader.structData.TK = FSGP_READY_FRAME;
    datagramHeader.structData.RK = __builtin_bswap32(datagramSize - FRAME_HEADER_SIZE);

    datagramHeader.structData.CTRL_OFFSET128 = __builtin_bswap32(FSGP_READYFRAME_CTRL_OFFSET128);
    datagramHeader.structData.CTRL_SIZE128 = __builtin_bswap32(FSGP_READYFRAME_CTRL_SIZE128);

    datagramHeader.structData.SYNC_OFFSET128 = __builtin_bswap32(FSGP_READYFRAME_SYNC_OFFSET);
    datagramHeader.structData.SYNC_SIZE128 = __builtin_bswap32(FSGP_READYFRAME_SYNC_SIZE);
    datagramHeader.structData.HEAD_AUX_OFFSET128 = __builtin_bswap32(FSGP_READYFRAME_AUX_OFFSET);
    datagramHeader.structData.HEAD_AUX_SIZE128 = __builtin_bswap32(FSGP_READYFRAME_AUX_SIZE);
    datagramHeader.structData.SIGNAL_OFFSET128 = __builtin_bswap32(FSGP_READYFRAME_SIGNAL_OFFSET);
    datagramHeader.structData.SIGNAL_SIZE128 = __builtin_bswap32(FSGP_READYFRAME_SIGNAL_SIZE);

    FSGP_READY_Frame readyFrame;

    memset(readyFrame.rawData, 0, FSGP_FDKFRAME_SIZE);

    memcpy(&data_ptr[0], frameHeader.rawData, FRAME_HEADER_SIZE);
    memcpy(&data_ptr[FRAME_HEADER_SIZE], datagramHeader.rawData, DATAGRAM_HEADER_SIZE);

    // Ready data
    readyFrame.structData.typeFSGP = 1;
    readyFrame.structData.gotRab = 1 + isRecievingControlFrames;
    memcpy(&data_ptr[FRAME_HEADER_SIZE + DATAGRAM_HEADER_SIZE], &(readyFrame.rawData), sizeof(FSGP_READY_Frame));

    if(dataLen_ptr) *dataLen_ptr = datagramSize;
}
