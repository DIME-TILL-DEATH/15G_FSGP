#ifndef USER_PROTOCOL_FSGP_FDK_FRAME_H_
#define USER_PROTOCOL_FSGP_FDK_FRAME_H_

#include <stdint.h>
#include "frame_defines.h"

#define FSGP_FDKFRAME_SIZE FSGP_FDKFRAME_CTRL_SIZE128 * WORD128_WORD32_SIZE * WORD32_BYTE_SIZE

// Fields value
#define FSGP_FDKFRAME_CTRL_OFFSET128 3
#define FSGP_FDKFRAME_CTRL_SIZE128 1

typedef struct
{
    //word1[32:0]
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;

    uint8_t OSFSGP      :1;
    uint8_t reserved4   :7;


    //word2[32:0]
    uint8_t reserved5;
    uint8_t reserved6;
    uint8_t reserved7;

    uint8_t OTSSFSGP      :1;
    uint8_t reserved8   :7;

    //word3[32:0]
    uint8_t reserved9;
    uint8_t reserved10;
    uint8_t reserved11;

    uint8_t PGFSGP      :1;
    uint8_t reserved12   :7;

    //word4[32:0]
    uint32_t reserved13;
}FSGP_FDK_Frame_struct;

typedef union
{
    FSGP_FDK_Frame_struct structData;
    uint8_t rawData[FSGP_FDKFRAME_SIZE];
}FSGP_FDK_Frame;

#endif /* USER_PROTOCOL_FSGP_FDK_FRAME_H_ */
