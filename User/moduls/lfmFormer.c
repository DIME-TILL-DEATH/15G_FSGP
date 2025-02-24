#include "lfmFormer.h"

#include "lfm_fifo.h"

#define STAGE1_LENGTH 0 * 24
#define STAGE2_LENGTH 4 * 24

GPIO_TypeDef* DATA_PORT;
ControlPin_t PIN_CS, PIN_ADR, PIN_WR, PIN_RD;

static inline void LFM_WriteReg(uint16_t address, uint16_t value);

LfmPack_t packData[PACK_COUNT+1] =
{
        // pack 0 - zero pack
        {
            .impLength = 0,
            .period = 24000,
            .impNum = 1
        },
        {
             .impLength = PACK1_IMP_LENGTH,
             .period = PACK1_PERIOD,
             .impNum = PACK1_IMP_NUM
        },
        {
             .impLength = PACK2_IMP_LENGTH,
             .period = PACK2_PERIOD,
             .impNum = PACK2_IMP_NUM
        },
        {
             .impLength = PACK3_IMP_LENGTH,
             .period = PACK3_PERIOD,
             .impNum = PACK3_IMP_NUM
        },
        {
             .impLength = PACK4_IMP_LENGTH,
             .period = PACK4_PERIOD,
             .impNum = PACK4_IMP_NUM
        },
        {
             .impLength = PACK5_IMP_LENGTH,
             .period = PACK5_PERIOD,
             .impNum = PACK5_IMP_NUM
        },
        {
             .impLength = PACK6_IMP_LENGTH,
             .period = PACK6_PERIOD,
             .impNum = PACK6_IMP_NUM
        },
        {
             .impLength = PACK7_IMP_LENGTH,
             .period = PACK7_PERIOD,
             .impNum = PACK7_IMP_NUM
        },
        {
             .impLength = PACK8_IMP_LENGTH,
             .period = PACK8_PERIOD,
             .impNum = PACK8_IMP_NUM
        },
        {
             .impLength = PACK9_IMP_LENGTH,
             .period = PACK9_PERIOD,
             .impNum = PACK9_IMP_NUM
        },
        {
             .impLength = PACK10_IMP_LENGTH,
             .period = PACK10_PERIOD,
             .impNum = PACK10_IMP_NUM
        },
        {
             .impLength = PACK11_IMP_LENGTH,
             .period = PACK11_PERIOD,
             .impNum = PACK11_IMP_NUM
        },
        {
             .impLength = PACK12_IMP_LENGTH,
             .period = PACK12_PERIOD,
             .impNum = PACK12_IMP_NUM
        },
        {
             .impLength = PACK13_IMP_LENGTH,
             .period = PACK13_PERIOD,
             .impNum = PACK13_IMP_NUM
        },
        {
             .impLength = PACK14_IMP_LENGTH,
             .period = PACK14_PERIOD,
             .impNum = PACK14_IMP_NUM
        },
        {
             .impLength = PACK15_IMP_LENGTH,
             .period = PACK15_PERIOD,
             .impNum = PACK15_IMP_NUM
        },
        {
             .impLength = PACK16_IMP_LENGTH,
             .period = PACK16_PERIOD,
             .impNum = PACK16_IMP_NUM
        },
        {
             .impLength = PACK17_IMP_LENGTH,
             .period = PACK17_PERIOD,
             .impNum = PACK17_IMP_NUM
        },
        {
             .impLength = PACK18_IMP_LENGTH,
             .period = PACK18_PERIOD,
             .impNum = PACK18_IMP_NUM
        },
        {
             .impLength = PACK19_IMP_LENGTH,
             .period = PACK19_PERIOD,
             .impNum = PACK19_IMP_NUM
        },
        {
             .impLength = PACK20_IMP_LENGTH,
             .period = PACK20_PERIOD,
             .impNum = PACK20_IMP_NUM
        },
        {
             .impLength = PACK21_IMP_LENGTH,
             .period = PACK21_PERIOD,
             .impNum = PACK21_IMP_NUM
        },
        {
             .impLength = PACK22_IMP_LENGTH,
             .period = PACK22_PERIOD,
             .impNum = PACK22_IMP_NUM
        },
        {
             .impLength = PACK23_IMP_LENGTH,
             .period = PACK23_PERIOD,
             .impNum = PACK23_IMP_NUM
        },
        {
             .impLength = PACK24_IMP_LENGTH,
             .period = PACK24_PERIOD,
             .impNum = PACK24_IMP_NUM
        },
        {
             .impLength = PACK25_IMP_LENGTH,
             .period = PACK25_PERIOD,
             .impNum = PACK25_IMP_NUM
        },
        {
             .impLength = PACK26_IMP_LENGTH,
             .period = PACK26_PERIOD,
             .impNum = PACK26_IMP_NUM
        },
        {
             .impLength = PACK27_IMP_LENGTH,
             .period = PACK27_PERIOD,
             .impNum = PACK27_IMP_NUM
        },
        {
             .impLength = PACK28_IMP_LENGTH,
             .period = PACK28_PERIOD,
             .impNum = PACK28_IMP_NUM
        },
        {
             .impLength = PACK29_IMP_LENGTH,
             .period = PACK29_PERIOD,
             .impNum = PACK29_IMP_NUM
        },
        {
             .impLength = PACK30_IMP_LENGTH,
             .period = PACK30_PERIOD,
             .impNum = PACK30_IMP_NUM
        },
        {
             .impLength = PACK31_IMP_LENGTH,
             .period = PACK31_PERIOD,
             .impNum = PACK31_IMP_NUM
        },
        {
             .impLength = PACK32_IMP_LENGTH,
             .period = PACK32_PERIOD,
             .impNum = PACK32_IMP_NUM
        },
        {
             .impLength = PACK33_IMP_LENGTH,
             .period = PACK33_PERIOD,
             .impNum = PACK33_IMP_NUM
        },
        {
             .impLength = PACK34_IMP_LENGTH,
             .period = PACK34_PERIOD,
             .impNum = PACK34_IMP_NUM
        },
        {
             .impLength = PACK35_IMP_LENGTH,
             .period = PACK35_PERIOD,
             .impNum = PACK35_IMP_NUM
        },
        {
             .impLength = PACK36_IMP_LENGTH,
             .period = PACK36_PERIOD,
             .impNum = PACK36_IMP_NUM
        },
        {
             .impLength = PACK37_IMP_LENGTH,
             .period = PACK37_PERIOD,
             .impNum = PACK37_IMP_NUM
        },
        {
             .impLength = PACK38_IMP_LENGTH,
             .period = PACK38_PERIOD,
             .impNum = PACK38_IMP_NUM
        },
        {
             .impLength = PACK39_IMP_LENGTH,
             .period = PACK39_PERIOD,
             .impNum = PACK39_IMP_NUM
        },
        {
             .impLength = PACK40_IMP_LENGTH,
             .period = PACK40_PERIOD,
             .impNum = PACK40_IMP_NUM
        },
        {
             .impLength = PACK41_IMP_LENGTH,
             .period = PACK41_PERIOD,
             .impNum = PACK41_IMP_NUM
        },
        {
             .impLength = PACK42_IMP_LENGTH,
             .period = PACK42_PERIOD,
             .impNum = PACK42_IMP_NUM
        },
        {
             .impLength = PACK43_IMP_LENGTH,
             .period = PACK43_PERIOD,
             .impNum = PACK43_IMP_NUM
        },
        {
             .impLength = PACK44_IMP_LENGTH,
             .period = PACK44_PERIOD,
             .impNum = PACK44_IMP_NUM
        },
        {
             .impLength = PACK45_IMP_LENGTH,
             .period = PACK45_PERIOD,
             .impNum = PACK45_IMP_NUM
        },
        {
             .impLength = PACK46_IMP_LENGTH,
             .period = PACK46_PERIOD,
             .impNum = PACK46_IMP_NUM
        },
        {
             .impLength = PACK47_IMP_LENGTH,
             .period = PACK47_PERIOD,
             .impNum = PACK47_IMP_NUM
        },
        {
             .impLength = PACK48_IMP_LENGTH,
             .period = PACK48_PERIOD,
             .impNum = PACK48_IMP_NUM
        },
        {
             .impLength = PACK49_IMP_LENGTH,
             .period = PACK49_PERIOD,
             .impNum = PACK49_IMP_NUM
        },
        {
             .impLength = PACK50_IMP_LENGTH,
             .period = PACK50_PERIOD,
             .impNum = PACK50_IMP_NUM
        },
        {
             .impLength = PACK51_IMP_LENGTH,
             .period = PACK51_PERIOD,
             .impNum = PACK51_IMP_NUM
        },
        {
             .impLength = PACK52_IMP_LENGTH,
             .period = PACK52_PERIOD,
             .impNum = PACK52_IMP_NUM
        },
        {
             .impLength = PACK53_IMP_LENGTH,
             .period = PACK53_PERIOD,
             .impNum = PACK53_IMP_NUM
        },
        {
             .impLength = PACK54_IMP_LENGTH,
             .period = PACK54_PERIOD,
             .impNum = PACK54_IMP_NUM
        },
        {
             .impLength = PACK55_IMP_LENGTH,
             .period = PACK55_PERIOD,
             .impNum = PACK55_IMP_NUM
        },
        {
             .impLength = PACK56_IMP_LENGTH,
             .period = PACK56_PERIOD,
             .impNum = PACK56_IMP_NUM
        },
        {
             .impLength = PACK57_IMP_LENGTH,
             .period = PACK57_PERIOD,
             .impNum = PACK57_IMP_NUM
        },
        {
             .impLength = PACK58_IMP_LENGTH,
             .period = PACK58_PERIOD,
             .impNum = PACK58_IMP_NUM
        },
        {
             .impLength = PACK59_IMP_LENGTH,
             .period = PACK59_PERIOD,
             .impNum = PACK59_IMP_NUM
        },
        {
             .impLength = PACK60_IMP_LENGTH,
             .period = PACK60_PERIOD,
             .impNum = PACK60_IMP_NUM
        },
        {
             .impLength = PACK61_IMP_LENGTH,
             .period = PACK61_PERIOD,
             .impNum = PACK61_IMP_NUM
        },
        {
             .impLength = PACK62_IMP_LENGTH,
             .period = PACK62_PERIOD,
             .impNum = PACK62_IMP_NUM
        },
        {
             .impLength = PACK63_IMP_LENGTH,
             .period = PACK63_PERIOD,
             .impNum = PACK63_IMP_NUM
        },
        {
             .impLength = PACK64_IMP_LENGTH,
             .period = PACK64_PERIOD,
             .impNum = PACK64_IMP_NUM
        },
        {
             .impLength = PACK65_IMP_LENGTH,
             .period = PACK65_PERIOD,
             .impNum = PACK65_IMP_NUM
        },
        {
             .impLength = PACK66_IMP_LENGTH,
             .period = PACK66_PERIOD,
             .impNum = PACK66_IMP_NUM
        },
        {
             .impLength = PACK67_IMP_LENGTH,
             .period = PACK67_PERIOD,
             .impNum = PACK67_IMP_NUM
        },
        {
             .impLength = PACK68_IMP_LENGTH,
             .period = PACK68_PERIOD,
             .impNum = PACK68_IMP_NUM
        },
        {
             .impLength = PACK69_IMP_LENGTH,
             .period = PACK69_PERIOD,
             .impNum = PACK69_IMP_NUM
        },
        {
             .impLength = PACK70_IMP_LENGTH,
             .period = PACK70_PERIOD,
             .impNum = PACK70_IMP_NUM
        },
        {
             .impLength = PACK71_IMP_LENGTH,
             .period = PACK71_PERIOD,
             .impNum = PACK71_IMP_NUM
        },
        {
             .impLength = PACK72_IMP_LENGTH,
             .period = PACK72_PERIOD,
             .impNum = PACK72_IMP_NUM
        },
        {
             .impLength = PACK73_IMP_LENGTH,
             .period = PACK73_PERIOD,
             .impNum = PACK73_IMP_NUM
        },
        {
             .impLength = PACK74_IMP_LENGTH,
             .period = PACK74_PERIOD,
             .impNum = PACK74_IMP_NUM
        },
        {
             .impLength = PACK75_IMP_LENGTH,
             .period = PACK75_PERIOD,
             .impNum = PACK75_IMP_NUM
        },
        {
             .impLength = PACK76_IMP_LENGTH,
             .period = PACK76_PERIOD,
             .impNum = PACK76_IMP_NUM
        },
        {
             .impLength = PACK77_IMP_LENGTH,
             .period = PACK77_PERIOD,
             .impNum = PACK77_IMP_NUM
        },
        {
             .impLength = PACK78_IMP_LENGTH,
             .period = PACK78_PERIOD,
             .impNum = PACK78_IMP_NUM
        },
        {
             .impLength = PACK79_IMP_LENGTH,
             .period = PACK79_PERIOD,
             .impNum = PACK79_IMP_NUM
        },
        {
             .impLength = PACK80_IMP_LENGTH,
             .period = PACK80_PERIOD,
             .impNum = PACK80_IMP_NUM
        },
        {
             .impLength = PACK81_IMP_LENGTH,
             .period = PACK81_PERIOD,
             .impNum = PACK81_IMP_NUM
        },
        {
             .impLength = PACK82_IMP_LENGTH,
             .period = PACK82_PERIOD,
             .impNum = PACK82_IMP_NUM
        },
        {
             .impLength = PACK83_IMP_LENGTH,
             .period = PACK83_PERIOD,
             .impNum = PACK83_IMP_NUM
        },
        {
             .impLength = PACK84_IMP_LENGTH,
             .period = PACK84_PERIOD,
             .impNum = PACK84_IMP_NUM
        },
        {
             .impLength = PACK85_IMP_LENGTH,
             .period = PACK85_PERIOD,
             .impNum = PACK85_IMP_NUM
        },
        {
             .impLength = PACK86_IMP_LENGTH,
             .period = PACK86_PERIOD,
             .impNum = PACK86_IMP_NUM
        },
        {
             .impLength = PACK87_IMP_LENGTH,
             .period = PACK87_PERIOD,
             .impNum = PACK87_IMP_NUM
        },
        {
             .impLength = PACK88_IMP_LENGTH,
             .period = PACK88_PERIOD,
             .impNum = PACK88_IMP_NUM
        },
        {
             .impLength = PACK89_IMP_LENGTH,
             .period = PACK89_PERIOD,
             .impNum = PACK89_IMP_NUM
        },
        {
             .impLength = PACK90_IMP_LENGTH,
             .period = PACK90_PERIOD,
             .impNum = PACK90_IMP_NUM
        },
        {
             .impLength = PACK91_IMP_LENGTH,
             .period = PACK91_PERIOD,
             .impNum = PACK91_IMP_NUM
        },
        {
             .impLength = PACK92_IMP_LENGTH,
             .period = PACK92_PERIOD,
             .impNum = PACK92_IMP_NUM
        },
        {
             .impLength = PACK93_IMP_LENGTH,
             .period = PACK93_PERIOD,
             .impNum = PACK93_IMP_NUM
        },
        {
             .impLength = PACK94_IMP_LENGTH,
             .period = PACK94_PERIOD,
             .impNum = PACK94_IMP_NUM
        },
        {
             .impLength = PACK95_IMP_LENGTH,
             .period = PACK95_PERIOD,
             .impNum = PACK95_IMP_NUM
        },
        {
             .impLength = PACK96_IMP_LENGTH,
             .period = PACK96_PERIOD,
             .impNum = PACK96_IMP_NUM
        },
        {
             .impLength = PACK97_IMP_LENGTH,
             .period = PACK97_PERIOD,
             .impNum = PACK97_IMP_NUM
        },
        {
             .impLength = PACK98_IMP_LENGTH,
             .period = PACK98_PERIOD,
             .impNum = PACK98_IMP_NUM
        },
        {
             .impLength = PACK99_IMP_LENGTH,
             .period = PACK99_PERIOD,
             .impNum = PACK99_IMP_NUM
        },
        {
            .impLength = PACK100_IMP_LENGTH,
            .period = PACK100_PERIOD,
            .impNum = PACK100_IMP_NUM
        },
        {
             .impLength = PACK101_IMP_LENGTH,
             .period = PACK101_PERIOD,
             .impNum = PACK101_IMP_NUM
        },
        {
             .impLength = PACK102_IMP_LENGTH,
             .period = PACK102_PERIOD,
             .impNum = PACK102_IMP_NUM
        },
        {
             .impLength = PACK103_IMP_LENGTH,
             .period = PACK103_PERIOD,
             .impNum = PACK103_IMP_NUM
        },
        {
             .impLength = PACK104_IMP_LENGTH,
             .period = PACK104_PERIOD,
             .impNum = PACK104_IMP_NUM
        },
        {
             .impLength = PACK105_IMP_LENGTH,
             .period = PACK105_PERIOD,
             .impNum = PACK105_IMP_NUM
        },
        {
             .impLength = PACK106_IMP_LENGTH,
             .period = PACK106_PERIOD,
             .impNum = PACK106_IMP_NUM
        },
        {
             .impLength = PACK107_IMP_LENGTH,
             .period = PACK107_PERIOD,
             .impNum = PACK107_IMP_NUM
        },
        {
             .impLength = PACK108_IMP_LENGTH,
             .period = PACK108_PERIOD,
             .impNum = PACK108_IMP_NUM
        },
        {
             .impLength = PACK109_IMP_LENGTH,
             .period = PACK109_PERIOD,
             .impNum = PACK109_IMP_NUM
        },
        {
             .impLength = PACK110_IMP_LENGTH,
             .period = PACK110_PERIOD,
             .impNum = PACK110_IMP_NUM
        },
        {
             .impLength = PACK111_IMP_LENGTH,
             .period = PACK111_PERIOD,
             .impNum = PACK111_IMP_NUM
        },
        {
             .impLength = PACK112_IMP_LENGTH,
             .period = PACK112_PERIOD,
             .impNum = PACK112_IMP_NUM
        },
        {
             .impLength = PACK113_IMP_LENGTH,
             .period = PACK113_PERIOD,
             .impNum = PACK113_IMP_NUM
        },
        {
             .impLength = PACK114_IMP_LENGTH,
             .period = PACK114_PERIOD,
             .impNum = PACK114_IMP_NUM
        },
        {
             .impLength = PACK115_IMP_LENGTH,
             .period = PACK115_PERIOD,
             .impNum = PACK115_IMP_NUM
        },
        {
             .impLength = PACK116_IMP_LENGTH,
             .period = PACK116_PERIOD,
             .impNum = PACK116_IMP_NUM
        },
        {
             .impLength = PACK117_IMP_LENGTH,
             .period = PACK117_PERIOD,
             .impNum = PACK117_IMP_NUM
        },
        {
             .impLength = PACK118_IMP_LENGTH,
             .period = PACK118_PERIOD,
             .impNum = PACK118_IMP_NUM
        },
        {
             .impLength = PACK119_IMP_LENGTH,
             .period = PACK119_PERIOD,
             .impNum = PACK119_IMP_NUM
        },
        {
             .impLength = PACK120_IMP_LENGTH,
             .period = PACK120_PERIOD,
             .impNum = PACK120_IMP_NUM
        },
        {
             .impLength = PACK121_IMP_LENGTH,
             .period = PACK121_PERIOD,
             .impNum = PACK121_IMP_NUM
        },
        {
             .impLength = PACK122_IMP_LENGTH,
             .period = PACK122_PERIOD,
             .impNum = PACK122_IMP_NUM
        },
        {
             .impLength = PACK123_IMP_LENGTH,
             .period = PACK123_PERIOD,
             .impNum = PACK123_IMP_NUM
        },
        {
             .impLength = PACK124_IMP_LENGTH,
             .period = PACK124_PERIOD,
             .impNum = PACK124_IMP_NUM
        },
        {
             .impLength = PACK125_IMP_LENGTH,
             .period = PACK125_PERIOD,
             .impNum = PACK125_IMP_NUM
        },
        {
             .impLength = PACK126_IMP_LENGTH,
             .period = PACK126_PERIOD,
             .impNum = PACK126_IMP_NUM
        },
        {
             .impLength = PACK127_IMP_LENGTH,
             .period = PACK127_PERIOD,
             .impNum = PACK127_IMP_NUM
        },
        {
             .impLength = PACK128_IMP_LENGTH,
             .period = PACK128_PERIOD,
             .impNum = PACK128_IMP_NUM
        },
        {
             .impLength = PACK129_IMP_LENGTH,
             .period = PACK129_PERIOD,
             .impNum = PACK129_IMP_NUM
        },
        {
             .impLength = PACK130_IMP_LENGTH,
             .period = PACK130_PERIOD,
             .impNum = PACK130_IMP_NUM
        },
        {
             .impLength = PACK131_IMP_LENGTH,
             .period = PACK131_PERIOD,
             .impNum = PACK131_IMP_NUM
        },
        {
             .impLength = PACK132_IMP_LENGTH,
             .period = PACK132_PERIOD,
             .impNum = PACK132_IMP_NUM
        },
        {
             .impLength = PACK133_IMP_LENGTH,
             .period = PACK133_PERIOD,
             .impNum = PACK133_IMP_NUM
        },
        {
             .impLength = PACK34_IMP_LENGTH,
             .period = PACK134_PERIOD,
             .impNum = PACK134_IMP_NUM
        },
        {
             .impLength = PACK135_IMP_LENGTH,
             .period = PACK135_PERIOD,
             .impNum = PACK135_IMP_NUM
        },
        {
             .impLength = PACK136_IMP_LENGTH,
             .period = PACK136_PERIOD,
             .impNum = PACK136_IMP_NUM
        },
        {
             .impLength = PACK137_IMP_LENGTH,
             .period = PACK137_PERIOD,
             .impNum = PACK137_IMP_NUM
        },
        {
             .impLength = PACK138_IMP_LENGTH,
             .period = PACK138_PERIOD,
             .impNum = PACK138_IMP_NUM
        },
        {
             .impLength = PACK139_IMP_LENGTH,
             .period = PACK139_PERIOD,
             .impNum = PACK139_IMP_NUM
        },
        {
             .impLength = PACK140_IMP_LENGTH,
             .period = PACK140_PERIOD,
             .impNum = PACK140_IMP_NUM
        },
        {
             .impLength = PACK141_IMP_LENGTH,
             .period = PACK141_PERIOD,
             .impNum = PACK141_IMP_NUM
        },
        {
             .impLength = PACK142_IMP_LENGTH,
             .period = PACK142_PERIOD,
             .impNum = PACK142_IMP_NUM
        },
        {
             .impLength = PACK143_IMP_LENGTH,
             .period = PACK143_PERIOD,
             .impNum = PACK143_IMP_NUM
        },
        {
             .impLength = PACK144_IMP_LENGTH,
             .period = PACK144_PERIOD,
             .impNum = PACK144_IMP_NUM
        },
        {
             .impLength = PACK145_IMP_LENGTH,
             .period = PACK145_PERIOD,
             .impNum = PACK145_IMP_NUM
        },
        {
             .impLength = PACK146_IMP_LENGTH,
             .period = PACK146_PERIOD,
             .impNum = PACK146_IMP_NUM
        },
        {
             .impLength = PACK147_IMP_LENGTH,
             .period = PACK147_PERIOD,
             .impNum = PACK147_IMP_NUM
        },
        {
             .impLength = PACK148_IMP_LENGTH,
             .period = PACK148_PERIOD,
             .impNum = PACK148_IMP_NUM
        },
        {
             .impLength = PACK149_IMP_LENGTH,
             .period = PACK149_PERIOD,
             .impNum = PACK149_IMP_NUM
        },
        {
             .impLength = PACK150_IMP_LENGTH,
             .period = PACK150_PERIOD,
             .impNum = PACK150_IMP_NUM
        },
        {
             .impLength = PACK151_IMP_LENGTH,
             .period = PACK151_PERIOD,
             .impNum = PACK151_IMP_NUM
        },
        {
             .impLength = PACK152_IMP_LENGTH,
             .period = PACK152_PERIOD,
             .impNum = PACK152_IMP_NUM
        },
        {
             .impLength = PACK153_IMP_LENGTH,
             .period = PACK153_PERIOD,
             .impNum = PACK153_IMP_NUM
        },
        {
             .impLength = PACK154_IMP_LENGTH,
             .period = PACK154_PERIOD,
             .impNum = PACK154_IMP_NUM
        },
        {
             .impLength = PACK155_IMP_LENGTH,
             .period = PACK155_PERIOD,
             .impNum = PACK155_IMP_NUM
        },
        {
             .impLength = PACK156_IMP_LENGTH,
             .period = PACK156_PERIOD,
             .impNum = PACK156_IMP_NUM
        },
        {
             .impLength = PACK157_IMP_LENGTH,
             .period = PACK157_PERIOD,
             .impNum = PACK157_IMP_NUM
        },
        {
             .impLength = PACK158_IMP_LENGTH,
             .period = PACK158_PERIOD,
             .impNum = PACK158_IMP_NUM
        },
        {
             .impLength = PACK159_IMP_LENGTH,
             .period = PACK159_PERIOD,
             .impNum = PACK159_IMP_NUM
        },
        {
             .impLength = PACK160_IMP_LENGTH,
             .period = PACK160_PERIOD,
             .impNum = PACK160_IMP_NUM
        },
        {
             .impLength = PACK167_IMP_LENGTH,
             .period = PACK167_PERIOD,
             .impNum = PACK167_IMP_NUM
        },
        {
             .impLength = PACK168_IMP_LENGTH,
             .period = PACK168_PERIOD,
             .impNum = PACK168_IMP_NUM
        },
        {
             .impLength = PACK169_IMP_LENGTH,
             .period = PACK169_PERIOD,
             .impNum = PACK169_IMP_NUM
        },
        {
             .impLength = PACK170_IMP_LENGTH,
             .period = PACK170_PERIOD,
             .impNum = PACK170_IMP_NUM
        },
        {
             .impLength = PACK171_IMP_LENGTH,
             .period = PACK171_PERIOD,
             .impNum = PACK171_IMP_NUM
        },
        {
             .impLength = PACK172_IMP_LENGTH,
             .period = PACK172_PERIOD,
             .impNum = PACK172_IMP_NUM
        },
        {
             .impLength = PACK173_IMP_LENGTH,
             .period = PACK173_PERIOD,
             .impNum = PACK173_IMP_NUM
        },
        {
             .impLength = PACK174_IMP_LENGTH,
             .period = PACK174_PERIOD,
             .impNum = PACK174_IMP_NUM
        },
        {
             .impLength = PACK175_IMP_LENGTH,
             .period = PACK175_PERIOD,
             .impNum = PACK175_IMP_NUM
        },
        {
             .impLength = PACK176_IMP_LENGTH,
             .period = PACK176_PERIOD,
             .impNum = PACK176_IMP_NUM
        },
        {
             .impLength = PACK177_IMP_LENGTH,
             .period = PACK177_PERIOD,
             .impNum = PACK177_IMP_NUM
        },
        {
             .impLength = PACK178_IMP_LENGTH,
             .period = PACK178_PERIOD,
             .impNum = PACK178_IMP_NUM
        },
        {
             .impLength = PACK179_IMP_LENGTH,
             .period = PACK179_PERIOD,
             .impNum = PACK179_IMP_NUM
        },
        {
             .impLength = PACK180_IMP_LENGTH,
             .period = PACK180_PERIOD,
             .impNum = PACK180_IMP_NUM
        },
        {
             .impLength = PACK181_IMP_LENGTH,
             .period = PACK181_PERIOD,
             .impNum = PACK181_IMP_NUM
        },
        {
             .impLength = PACK182_IMP_LENGTH,
             .period = PACK182_PERIOD,
             .impNum = PACK182_IMP_NUM
        },
        {
             .impLength = PACK183_IMP_LENGTH,
             .period = PACK183_PERIOD,
             .impNum = PACK183_IMP_NUM
        },
        {
             .impLength = PACK184_IMP_LENGTH,
             .period = PACK184_PERIOD,
             .impNum = PACK184_IMP_NUM
        },
        {
             .impLength = PACK185_IMP_LENGTH,
             .period = PACK185_PERIOD,
             .impNum = PACK185_IMP_NUM
        },
        {
             .impLength = PACK186_IMP_LENGTH,
             .period = PACK186_PERIOD,
             .impNum = PACK186_IMP_NUM
        },
        {
             .impLength = PACK187_IMP_LENGTH,
             .period = PACK187_PERIOD,
             .impNum = PACK187_IMP_NUM
        },
        {
             .impLength = PACK188_IMP_LENGTH,
             .period = PACK188_PERIOD,
             .impNum = PACK188_IMP_NUM
        },
        {
             .impLength = PACK189_IMP_LENGTH,
             .period = PACK189_PERIOD,
             .impNum = PACK189_IMP_NUM
        },
        {
             .impLength = PACK190_IMP_LENGTH,
             .period = PACK190_PERIOD,
             .impNum = PACK190_IMP_NUM
        },
        {
             .impLength = PACK191_IMP_LENGTH,
             .period = PACK191_PERIOD,
             .impNum = PACK191_IMP_NUM
        },
        {
             .impLength = PACK192_IMP_LENGTH,
             .period = PACK192_PERIOD,
             .impNum = PACK192_IMP_NUM
        },
        {
             .impLength = PACK193_IMP_LENGTH,
             .period = PACK193_PERIOD,
             .impNum = PACK193_IMP_NUM
        },
        {
             .impLength = PACK194_IMP_LENGTH,
             .period = PACK194_PERIOD,
             .impNum = PACK194_IMP_NUM
        },
        {
             .impLength = PACK195_IMP_LENGTH,
             .period = PACK195_PERIOD,
             .impNum = PACK195_IMP_NUM
        },
        {
             .impLength = PACK196_IMP_LENGTH,
             .period = PACK196_PERIOD,
             .impNum = PACK196_IMP_NUM
        },
};


void TIM6_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));

/********************************************************
 * delay - in discrets 24MHz
 * doppler - in MHz
 *******************************************************/
DdsRegisterData_t LFM_CalcPackData(LfmPack_t pack, bool isPositiveLfm, double_t delay, double_t dopplerFreq)
{
    DdsRegisterData_t outputData = {0};

    uint64_t freqStart;

    uint64_t dF;
    if(isPositiveLfm)
    {
        freqStart = DDS1508_CalcFreqWord(FSTART+dopplerFreq);
        dF = DDS1508_CalcDFWord(FSTART, FSTOP, pack.impLength);

    }
    else
    {
        freqStart = DDS1508_CalcFreqWord(FSTOP+dopplerFreq);
        dF = DDS1508_CalcDFWord(FSTOP, FSTART, pack.impLength);
    }

    outputData.startF[0] = (freqStart & 0xFFFF);
    outputData.startF[1] = (freqStart & 0xFFFF0000) >> 16;
    outputData.startF[2] = (freqStart & 0xFFFF00000000) >> 32;

    outputData.deltaF[0] = ((uint64_t)dF & 0xFFFF);
    outputData.deltaF[1] = ((uint64_t)dF & 0xFFFF0000) >> 16;
    outputData.deltaF[2] = ((uint64_t)dF & 0xFFFF00000000) >> 32;

    double_t stage1_length = STAGE1_LENGTH + delay;
    uint64_t tph1 = DDS1508_CalcTWord(stage1_length);
    outputData.tph1[0] = (tph1 & 0xFFFF);
    outputData.tph1[1] = (tph1 & 0xFFFF0000) >> 16;
    outputData.tph1[2] = (tph1 & 0xFFFF00000000) >> 32;

    double_t stage2_length = STAGE2_LENGTH;
    uint64_t tph2 = DDS1508_CalcTWord(stage2_length);
    outputData.tph2[0] = (tph2 & 0xFFFF);
    outputData.tph2[1] = (tph2 & 0xFFFF0000) >> 16;
    outputData.tph2[2] = (tph2 & 0xFFFF00000000) >> 32;

    uint64_t tph3 = DDS1508_CalcTWord(pack.impLength);
    outputData.tph3[0] = (tph3 & 0xFFFF);
    outputData.tph3[1] = (tph3 & 0xFFFF0000) >> 16;
    outputData.tph3[2] = (tph3 & 0xFFFF00000000) >> 32;

    double_t lengthStage4 = pack.period - pack.impLength - stage1_length - stage2_length;
    uint64_t tph4 = DDS1508_CalcTWord(lengthStage4);
    outputData.tph4[0] = (tph4 & 0xFFFF);
    outputData.tph4[1] = (tph4 & 0xFFFF0000) >> 16;
    outputData.tph4[2] = (tph4 & 0xFFFF00000000) >> 32;

    return outputData;
}

void LFM_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    // Data[0:15]
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    DATA_PORT = GPIOE;

    // CS, ADR, WR, RD
    PIN_CS.port = PIN_ADR.port = PIN_WR.port = PIN_RD.port = GPIOD;
    PIN_CS.pin = GPIO_Pin_12;
    PIN_ADR.pin = GPIO_Pin_13;
    PIN_WR.pin = GPIO_Pin_14;
    PIN_RD.pin = GPIO_Pin_15;

    GPIO_InitStructure.GPIO_Pin = PIN_CS.pin | PIN_ADR.pin | PIN_WR.pin | PIN_RD.pin;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(PIN_CS.port, PIN_CS.pin);
    GPIO_SetBits(PIN_ADR.port, PIN_ADR.pin);
    GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
    GPIO_SetBits(PIN_RD.port, PIN_RD.pin);

    DDS1508_SetDiscretisationFreq(FDISCRET);

    LfmFIFO_Init();
    LFM_WriteStartupData();
}

/*
void LFM_RecalcImitData(double_t delay, double_t dopplerFreq)
{
    for(uint16_t i=0; i<PACK_COUNT+1; i++)
    {
        ddsPackData[PACK_COUNT + 1 + i] = calcPackData(packData[i], delay, dopplerFreq);
    }
}
*/
// Fast routine
static inline void LFM_WriteReg(uint16_t address, uint16_t value)
{
    PIN_ADR.port->BCR = PIN_ADR.pin;
    PIN_WR.port->BCR = PIN_WR.pin;
    DATA_PORT->OUTDR =  address;
    PIN_WR.port->BSHR = PIN_WR.pin;
    PIN_ADR.port->BSHR = PIN_ADR.pin;

    PIN_WR.port->BCR = PIN_WR.pin;
    DATA_PORT->OUTDR = value;
    PIN_WR.port->BSHR = PIN_WR.pin;
}

void LFM_WriteStartupData()
{
    PIN_CS.port->BCR = PIN_CS.pin;

    LFM_WriteReg(DDS1508_ADDR_SWRST, 0x0078);
    LFM_WriteReg(DDS1508_ADDR_CTR, 0x1000);
    LFM_WriteReg(DDS1508_ADDR_SYNC, 0x4182);
    LFM_WriteReg(DDS1508_ADDR_ROUTE, 0x0000);

    DdsRegisterData_t ddsData = LFM_CalcPackData(packData[0], 1, 0, 0);

    LFM_WriteReg(DDS1508_ADDR_CH1_F_H, ddsData.startF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_M, ddsData.startF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_L, ddsData.startF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_dF_H, ddsData.deltaF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_M, ddsData.deltaF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_L, ddsData.deltaF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH1_L, ddsData.tph1[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH2_L, ddsData.tph2[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH3_L, ddsData.tph3[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH4_L, ddsData.tph4[0]);

    LFM_WriteReg(DDS1508_ADDR_CLR, 0x003F);
    LFM_WriteReg(DDS1508_ADDR_CH1_LS_CTR, 0xBC10);

    PIN_CS.port->BSHR = PIN_CS.pin;
}

void LFM_SetPack(DdsRegisterData_t* ddsData)
{
    PIN_CS.port->BCR = PIN_CS.pin;
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH1_L, ddsData->tph1[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_F_H, ddsData->startF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_M, ddsData->startF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_F_L, ddsData->startF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_dF_H, ddsData->deltaF[2]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_M, ddsData->deltaF[1]);
    LFM_WriteReg(DDS1508_ADDR_CH1_dF_L, ddsData->deltaF[0]);

    LFM_WriteReg(DDS1508_ADDR_CH1_TPH3_L, ddsData->tph3[0]);
    LFM_WriteReg(DDS1508_ADDR_CH1_TPH4_L, ddsData->tph4[0]);

    PIN_CS.port->BSHR = PIN_CS.pin;
}
//------------------------------------------------------------------------------
// bufferred (not used)
//------------------------------------------------------------------------------

void LFM_SetPackBuffered(DdsRegisterData_t* ddsData)
{
    DDS1508_Command_t comm;

    comm.address = DDS1508_ADDR_CH1_F_H;
    comm.value = ddsData->startF[2];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_F_M;
    comm.value = ddsData->startF[1];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_F_L;
    comm.value = ddsData->startF[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_H;
    comm.value = ddsData->deltaF[2];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_M;
    comm.value = ddsData->deltaF[1];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_dF_L;
    comm.value = ddsData->deltaF[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_TPH3_L;
    comm.value = ddsData->tph3[0];
    LfmFIFO_PutData(comm);

    comm.address = DDS1508_ADDR_CH1_TPH4_L;
    comm.value = ddsData->tph4[0];
    LfmFIFO_PutData(comm);

    NVIC_EnableIRQ(TIM6_IRQn);
}

DDS1508_Command_t actualComm;
LfmSendState_t sendingState = IDLE;
void TIM6_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

    switch(sendingState)
    {
    case IDLE:
        if(LfmFIFO_Count() > 0)
        {
            actualComm = LfmFIFO_GetData();
            GPIO_ResetBits(PIN_CS.port, PIN_CS.pin);
            sendingState = ADR_SET;
        }
        else
        {
            GPIO_SetBits(PIN_CS.port, PIN_CS.pin);
            NVIC_DisableIRQ(TIM6_IRQn);
        }
        break;

    case ADR_SET:
        GPIO_Write(DATA_PORT, actualComm.address);
        GPIO_ResetBits(PIN_ADR.port, PIN_ADR.pin);
        GPIO_ResetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = ADR_WR;
        break;

    case ADR_WR:
        GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
        GPIO_SetBits(PIN_ADR.port, PIN_ADR.pin);
        sendingState = DATA_SET;
        break;

    case DATA_SET:
        GPIO_Write(DATA_PORT, actualComm.value);
        GPIO_ResetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = DATA_WR;
        break;

    case DATA_WR:
        GPIO_SetBits(PIN_WR.port, PIN_WR.pin);
        sendingState = IDLE;
        break;
    }
}
