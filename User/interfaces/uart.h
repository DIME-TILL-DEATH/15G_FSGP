#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v30x.h"

typedef enum
{
    UART_NUM1,
    UART_NUM2
}UART_Type;

void UART_Init(void);
void UART_WriteData(UART_Type uartType, uint8_t* data_ptr, uint16_t len);
void UART_WriteByte(UART_Type uartType, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
