/*
 * fsgp_ready_frame.h
 *
 *  Created on: Feb 25, 2025
 *      Author: uaas
 */

#ifndef USER_PROTOCOL_FSGP_READY_FRAME_H_
#define USER_PROTOCOL_FSGP_READY_FRAME_H_

#include <stdint.h>
#include "frame_defines.h"

#define FSGP_READYFRAME_SIZE (FSGP_READYFRAME_CTRL_SIZE128 + FSGP_READYFRAME_SYNC_SIZE + FSGP_READYFRAME_AUX_SIZE + FSGP_READYFRAME_SIGNAL_SIZE) * WORD128_WORD32_SIZE * WORD32_BYTE_SIZE

// Fields value
#define FSGP_READYFRAME_CTRL_OFFSET128 3
#define FSGP_READYFRAME_CTRL_SIZE128 0

#define FSGP_READYFRAME_SYNC_OFFSET 3
#define FSGP_READYFRAME_SYNC_SIZE 0

#define FSGP_READYFRAME_AUX_OFFSET 3
#define FSGP_READYFRAME_AUX_SIZE 1

#define FSGP_READYFRAME_SIGNAL_OFFSET 4
#define FSGP_READYFRAME_SIGNAL_SIZE 0

typedef struct
{
    // word 16, [31:0], offset 0x6a
    uint8_t reserved_W16_3;
    uint8_t reserved_W16_2;
    uint8_t gotRab;
    uint8_t typeFSGP;

    uint32_t reserved_W17;
    uint32_t reserved_W18;
    uint32_t reserved_W19;
}FSGP_READY_Frame_struct;

typedef union
{
    FSGP_READY_Frame_struct structData;
    uint8_t rawData[FSGP_FDKFRAME_SIZE];
}FSGP_READY_Frame;

#endif /* USER_PROTOCOL_FSGP_READY_FRAME_H_ */
