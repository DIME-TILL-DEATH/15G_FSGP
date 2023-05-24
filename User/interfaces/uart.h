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

void uart_init(void);
void uart_write_data(UART_Type uartType, uint8_t* data_ptr, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
