#ifndef USER_PROTOCOL_FSGP_FDK_FRAME_H_
#define USER_PROTOCOL_FSGP_FDK_FRAME_H_

#include <stdint.h>
#include "frame_defines.h"

#define FSGP_FDKFRAME_SIZE (FSGP_FDKFRAME_CTRL_SIZE128 + FSGP_FDKFRAME_SYNC_SIZE + FSGP_FDKFRAME_AUX_SIZE + FSGP_FDKFRAME_SIGNAL_SIZE) * WORD128_WORD32_SIZE * WORD32_BYTE_SIZE

// Fields value
#define FSGP_FDKFRAME_CTRL_OFFSET128 3
#define FSGP_FDKFRAME_CTRL_SIZE128 4

#define FSGP_FDKFRAME_SYNC_OFFSET 7
#define FSGP_FDKFRAME_SYNC_SIZE 4

#define FSGP_FDKFRAME_AUX_OFFSET 11
#define FSGP_FDKFRAME_AUX_SIZE 8 //3

#define FSGP_FDKFRAME_SIGNAL_OFFSET 14
#define FSGP_FDKFRAME_SIGNAL_SIZE 0

typedef struct
{
    // word 16, [31:24], offset 0x6a
   uint8_t RVCH         :2;       //§²§£§¹
   uint8_t reserved1    :3;
   uint8_t PTVRS        :1;     // §±§´§£§²§³
   uint8_t SBR_OCH      :1;     //§³§¢§² §°§¹
   uint8_t REG          :1;     //§²§¦§¤

   // word 16, [23:16], offset 0x6b
   uint8_t NKCH         :8;     //§¯§¬§¹

   // word 16, [15:8]
   uint8_t NPO          :4;     //§¯§±§°
   uint8_t NVt          :1;     //§¯§£§´
   uint8_t reserved2    :3;

   // word 16, [7:0]
   uint8_t KP           :8;     // §¬§± - §¬§à§Õ §á§Ñ§é§Ü§Ú

   // word 17, [31:24]
   uint8_t MAKC         :8;     //§®§¡§¬§¸

   // word 17, [23:16]
   uint8_t IZL          :1;     //§ª§©§­
   uint8_t SZIZL        :1;     //§³§ª§©§­
   uint8_t FZI          :1;     //§¶§©§ª
   uint8_t reserved3    :1;
   uint8_t ComPCH       :1;     //§¬§à§Þ §±§¹
   uint8_t PRKF         :1;     //§±§²§¬§¶
   uint8_t ComPS        :1;     //§¬§à§Þ §±§³
   uint8_t reserved4    :1;

   // word 17, [15:8]
   uint8_t MO           :1;     //§®§°
   uint8_t PG           :1;     //§±§¤
   uint8_t CHPK         :1;     //§¹§±§¬
   uint8_t reserved5    :5;

   // word 17, [7:0]
   uint8_t PS           :1;     //§±§³
   uint8_t CDO          :1;     //§¸§¥§°
   uint8_t PBL          :1;     //§±§¢§­
   uint8_t PF           :1;     //§±§¶
   uint8_t AKP          :1;     //§¡§¬§±
   uint8_t EKV          :1;     //§¿§¬§£
   uint8_t NLCHM        :1;     //§¯§­§¹§®
   uint8_t ADAP         :1;     //§¡§¥§¡§±

   //word 18, [31:24]
   uint8_t reserved7;

   //word 18, [23:16]
   uint8_t reserved8;

   //word 18, [15:8]
   uint8_t reserved9;

   //word 18, [7:0]
   uint8_t ImAz         :1; //§ª§Þ§¡§Ù
   uint8_t VRASH        :1; //§£§²§¡§»
   uint8_t SkVRASH      :3; //§³§Ü§£§²§¡§»
   uint8_t reserved10   :3;

   //word 19, [31:0]
   uint32_t DVRS;  //§¥§Ý§Ú§ä§Ö§Ý§î§ß§à§ã§ä§î §£§²§³

   //word 20, [31:16]
   uint16_t DalnostPS;  //§¥§Ñ§Ý§î§ß§à§ã§ä§î §±§³

   //word 20, [16:8]
   uint8_t OslableniePS;  //§°§ã§Ý§Ñ§Ò§Ý§Ö§ß§Ú§Ö §±§³

   //word 20, [7:0]
   uint8_t TipPS        :3; //§°§ã§Ý§Ñ§Ò§Ý§Ö§ß§Ú§Ö §±§³
   uint8_t PolosaPS     :3; //§±§à§Ý§à§ã§Ñ §±§³
   uint8_t PnfPS        :1; //§±§¯§¶ §±§³
   uint8_t reserved11   :1;

   //word 21, [31:24]
   uint8_t reserved12;

   //word 21, [23:16]
   uint8_t reserved13   :4;
   uint8_t PNF_ZI       :1; //§±§¯§¶ §©§ª
   uint8_t PolosaZI     :3; //§±§à§Ý§à§ã§Ñ §©§ª

   //word 21, [15:0]
   uint16_t SkorostPS;  //§³§Ü§à§â§à§ã§ä§î §±§³

   //word 22 [32:0]
   uint8_t KFprdK1  :8;
   uint8_t KFprdK2  :8;
   uint8_t KFprdK3  :8;
   uint8_t KFprdK4  :8;

   //word 23, [32:0]
   uint16_t Xpl;
   uint16_t KPK;

   //word 24... word 31
   uint16_t X[16];  //X1-X16

   //word 32, [32:0]
   uint32_t index;

   //word 33, [32:0]
   uint32_t TVRS;  //§´§Ó§â§ã

   //word 34, [32:0]
   uint16_t TNSO;  //§´§Ö§Ü§å§ë§Ú§Û §ß§à§Þ§Ö§â §ã§Ü§Ñ§ß§Ñ §Ó §à§Ò§Ù§à§â§Ö
   uint16_t TNO;  //§´§Ö§Ü§å§ë§Ú§Û §ß§à§Þ§Ö§â §à§Ò§Ù§à§â§Ñ

    //word 35, [32:0]
    uint32_t azimutUPR;

    //word 36, [32:0]
    uint32_t azimutSDna4;

    //word 37, [32:0]
    uint32_t azimutSDkon;

    //word 38, [32:0]
    uint32_t Tev63_32;

    //word 39, [32:0]
    uint32_t Tev31_0;

    //word 40, [32:0]
    uint32_t TNS;

   //word 41, [32:0]
   uint32_t timestamp_hw;

   //word 42, [32:0]
   uint32_t timestamp_mw;

   //word 43, [32:0]
   uint32_t timestamp_lw;

   //words 44-47
   uint32_t reserved_W44_W47[4];

   //word48, [32:8]
   uint8_t PV_KFDS_FSGP;
   uint8_t reserved_W48_2;
   uint8_t reserved_W48_1;

   //word 48, [7:0]
   uint8_t SglF     :2;
   uint8_t MS_ZI    :1;
   uint8_t reserved_W48_0;

   //words 49-51
   uint32_t reserved_W49_W51[3];

   uint16_t NFZI[40];

   //word 52, [31:16]
   uint8_t temperature;
   uint8_t reserved_W52_2;

   //word 52, [15:8]
   uint8_t AvFSGP   :1;
   uint8_t UsVid    :1;
   uint8_t PROjd    :1;
   uint8_t reserved_W52_1 :5;

   //word 52, [7:0]
   uint8_t VnAvFSGP     :1;
   uint8_t AvRG1        :1;
   uint8_t AvRG2        :1;
   uint8_t AvRPS        :1;
   uint8_t AvRZI        :1;
   uint8_t AvUPR        :1;
   uint8_t AvNP         :1;

   //word 53, [32:0]
   uint16_t reserved_W53_32;
   uint16_t S4K_VnAvFSGP;

   //word 54, [32:0]
   uint16_t S4K_AvRG2;
   uint16_t S4K_AvRG1;

   //word 55, [32:0]
   uint16_t S4K_AvRZI;
   uint16_t S4K_AvRPS;

   //word 56, [32:0]
   uint16_t S4K_NPbKU;
   uint16_t S4K_KUbNP;

   //word 57, [32:0]
   uint16_t S4K_AvNPr;
   uint16_t S4K_CountTdAvNPr;

   //word 58, [32:0]
   uint16_t S4K_AvNPp;
   uint16_t S4K_CountTdAvNPp;

   //word59, [32:0]
   uint16_t reserved_W59_32;
   uint16_t S4K_AVTvrs;

}FSGP_FDK_Frame_struct;

typedef union
{
    FSGP_FDK_Frame_struct structData;
    uint8_t rawData[FSGP_FDKFRAME_SIZE];
}FSGP_FDK_Frame;

#endif /* USER_PROTOCOL_FSGP_FDK_FRAME_H_ */
