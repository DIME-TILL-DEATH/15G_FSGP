#ifndef USER_PROTOCOL_FSGP_COMMAND_FRAME_H_
#define USER_PROTOCOL_FSGP_COMMAND_FRAME_H_

#include <stdint.h>
#include "frame_defines.h"

#define FSGP_BUFFER_SIZE_LW_POS FRAME_HEADER_SIZE + 24*WORD32_BYTE_SIZE + LOW_WORD_OFFSET //115
#define FSGP_QUEUE_SIZE_LW_POS FRAME_HEADER_SIZE + 26*WORD32_BYTE_SIZE + LOW_WORD_OFFSET //123

// Fields value
#define FSGP_ACKFRAME_CTRL_OFFSET128 3
#define FSGP_ACKFRAME_CTRL_SIZE128 2

#define FSGP_ACKFRAME_SYNC_OFFSET128 5
#define FSGP_ACKFRAME_SYNC_SIZE128 1

#define FSGP_ACKFRAME_AUX_DATA_OFFSET128 6
#define FSGP_ACKFRAME_AUX_DATA_SIZE128 1

#define FSGP_ACKFRAME_SIGNAL_OFFSET128 7
#define FSGP_ACKFRAME_SIGNAL_SIZE128 0

typedef struct
{
    // word 12, [31:24], offset 0x6a
   uint8_t reserved1  :6;
   uint8_t SBR_OCH    :1;     //§³§¢§² §°§¹
   uint8_t REG        :1;     //§²§¦§¤

   // word 12, [23:16], offset 0x6b
   uint8_t NKCH       :8;     //§¯§¬§¹

   // word 12, [15:8]
   uint8_t NPO        :4;     //§¯§±§°
   uint8_t reserved2  :4;

   // word 12, [7:0]
   uint8_t KP         :8;     // §¬§à§Õ §á§Ñ§é§Ü§Ú

   // word 13, [31:24]
   uint8_t reserved3  :8;

   // word 13, [23:16]
   uint8_t ZI         :4;     //§©§ª
   uint8_t reserved4  :4;

   // word 13, [15:8]
   uint8_t reserved5  :8;

   // word 13, [7:0]
   uint8_t PS         :1;     //§±§³
   uint8_t CDO        :1;     //§¸§¥§°
   uint8_t PBL        :1;     //§±§¢§­
   uint8_t PF         :1;     //§±§¶
   uint8_t AKP        :1;     //§¡§¬§±
   uint8_t EKV        :1;     //§¿§¬§£
   uint8_t reserved6  :2;

   //word 14, [32:0]
   uint32_t index; // §ª§ß§Õ§Ö§Ü§ã

   //word 15, [32:0]
   uint32_t TVRS;  // §´§Ó§â§ã

   //word 16, [32:0]
   uint32_t reserved7;

   //word 17, [32:0]
   uint32_t reserved8;

   //word 18
   uint8_t reserved9  :8;
   uint8_t reserved10     :8;
   uint8_t reserved11     :8;
   uint8_t OSL_PS     :8;     // §°§ã§Ý.§±§ã

   //word 19, [32:0]
   uint32_t reserved12;

   //word 20, [32:0]
   uint32_t reserved13;

   //word 21, [32:0]
   uint32_t timestamp_hw;

   //word 22, [32:0]
   uint32_t timestamp_mw;

   //word 23, [32:0]
   uint32_t timestamp_lw;
}FSGP_Command_Frame;

#endif /* USER_PROTOCOL_FSGP_COMMAND_FRAME_H_ */
