#ifndef USER_VEEPROM_H_
#define USER_VEEPROM_H_

#include <debug.h>

typedef struct
{
    uint16_t serial;
    uint8_t dataByte;
}SavedData;

void VEEPROM_Init();

SavedData VEEPROM_GetSavedData();
void VEEPROM_SaveData(const SavedData* dataStruct);

#endif /* USER_VEEPROM_H_ */
