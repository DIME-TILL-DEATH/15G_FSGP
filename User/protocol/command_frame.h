/*
 * command_frame.h
 *
 *  Created on: May 24, 2023
 *      Author: Kostyuvhik Dmitriy
 */

#ifndef USER_PROTOCOL_COMMAND_FRAME_H_
#define USER_PROTOCOL_COMMAND_FRAME_H_

#include <stdint.h>

#define COMMAND_FRAME_POS 3
#define COMMAND_FRAME 0x3

#define COMMAND_DATA_POS 64

#define OSL_PS_POS 91

#define BUFFER_SIZE_LW_POS 115
#define QUEUE_SIZE_LW_POS 123

typedef struct
{
    // word 12, [31:24]
   uint8_t reserved1  :6;
   uint8_t SBR_OCH    :1;     //§³§¢§² §°§¹
   uint8_t REG        :1;     //§²§¦§¤

   // word 12, [23:16]
   uint8_t NKCH       :8;     //§¯§¬§¹

   // word 12, [15:8]
   uint8_t NPO        :4;     //§¯§±§°
   uint8_t reserved2  :4;

   // word 12, [7:0]
   uint8_t KP         :8;     // §¬§±

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
}Command_Frame;

#endif /* USER_PROTOCOL_COMMAND_FRAME_H_ */
