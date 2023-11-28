#ifndef USER_INTERFACES_SPI_HETERODINE_H_
#define USER_INTERFACES_SPI_HETERODINE_H_

#include "debug.h"

#include <stdbool.h>

#include "dds1508.h"

typedef enum
{
    HET_CHANNEL1 = 1,
    HET_CHANNEL2
}SPIHET_Channel_t;

typedef struct
{
    SPIHET_Channel_t channel;
    uint16_t regAddress;
    uint16_t* data_ptr;
    uint16_t dataLen;
}SPIHET_SendData_t;

void SPIHET_Init();
void SPIHET_PutDataInSendBuffer(const SPIHET_SendData_t* data);
uint8_t SPIHET_CommCount();
void SPIHET_ProcessSpiData();

void SPIHET_Task();


#endif /* USER_INTERFACES_SPI_HETERODINE_H_ */
