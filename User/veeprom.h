#ifndef USER_VEEPROM_H_
#define USER_VEEPROM_H_

#include <debug.h>

typedef struct
{
    uint16_t serial;
    uint8_t dataByte;
}SaveData;

void VEEPROM_Init();

SaveData VEEPROM_GetSavedData();
void VEEPROM_SaveData(const SaveData* dataStruct);

#endif /* USER_VEEPROM_H_ */
