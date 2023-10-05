#ifndef USER_PROTOCOL_FRAME_DEFINES_H_
#define USER_PROTOCOL_FRAME_DEFINES_H_

#define LOW_WORD_OFFSET 3

#define WORD32_BYTE_SIZE 4
#define WORD128_WORD32_SIZE 4

#define FRAME_HEADER_SIZE 4*WORD32_BYTE_SIZE   // four 32 bit words
#define DATAGRAM_HEADER_SIZE 12*WORD32_BYTE_SIZE

#define HEADER_FRAME_TYPE_POS 3
#define BODY_FRAME_TYPE_POS 19

#define FSGP_COMMAND_FRAME 3
#define FSGP_ACK_FRAME 4
#define UPPM_COMMAND_FRAME 5
#define UPPM_ACK_FRAME 6
#define FSGP_SIGNAL_DESCR_FRAME 8
#define FSGP_PACK_DESCR_FRAME 9
#define UPPM_FDK_FRAME 15
#define FSGP_FDK_FRAME 16

#define COMMAND_DATA_POS FRAME_HEADER_SIZE+DATAGRAM_HEADER_SIZE

//-------------------------------------------------
#define FRAME_SIGNATURE 0xACAB

typedef struct
{
    uint16_t signature;
    uint8_t RTK;
    uint8_t TK;

    uint32_t RK;

    uint8_t SCH;

    uint8_t PF          :1;
    uint8_t reserved1   :7;

    uint16_t RF128;
    uint32_t NF;
}Frame_Header_Struct;

typedef union
{
    Frame_Header_Struct structData;
    uint8_t rawData[FRAME_HEADER_SIZE];
}FrameHeader;

//-------------------------------------------------
typedef struct
{
    uint8_t LAYOUT;
    uint8_t LAYOUT_SIZE128;
    uint8_t RTK;
    uint8_t TK;

    uint32_t RK;

    uint32_t reserved1;
    uint32_t reserved2;

    uint32_t CTRL_OFFSET128;
    uint32_t CTRL_SIZE128;

    uint32_t SYNC_OFFSET128;
    uint32_t SYNC_SIZE128;

    uint32_t HEAD_AUX_OFFSET128;
    uint32_t HEAD_AUX_SIZE128;

    uint32_t SIGNAL_OFFSET128;
    uint32_t SIGNAL_SIZE128;
}Datagram_Header_Struct;

typedef union
{
    Datagram_Header_Struct structData;
    uint8_t rawData[DATAGRAM_HEADER_SIZE];
}DatagramHeader;
//-------------------------------------------------
#endif /* USER_PROTOCOL_FRAME_DEFINES_H_ */
