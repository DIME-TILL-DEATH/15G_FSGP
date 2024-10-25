#ifndef USER_INTERFACES_SPI_HETERODINE_H_
#define USER_INTERFACES_SPI_HETERODINE_H_

#include "debug.h"

#include <stdbool.h>

#include "control_pin.h"

#define AD9912_COMMAND_SIZE 2

typedef enum
{
    HET_CHANNEL1 = 1,
    HET_CHANNEL2
}SPIHET_Channel_t;

typedef struct
{
    SPIHET_Channel_t channel;
    uint16_t regAddress;
    uint8_t* data_ptr;
    uint16_t dataLen;
}SPIHET_SendData_t;

void SPIHET_Init();
void SPIHET_PutDataInSendBuffer(const SPIHET_SendData_t* data);
void SPIHET_ProcessSpiFifo();
void SPIHET_SendSpiData(SPIHET_SendData_t* data_ptr);

void SPIHET_LockPins();
void SPIHET_UnlockPins();

uint8_t SPIHET_CommCount();

#endif /* USER_INTERFACES_SPI_HETERODINE_H_ */
