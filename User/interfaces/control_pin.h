#ifndef USER_INTERFACES_CONTROL_PIN_H_
#define USER_INTERFACES_CONTROL_PIN_H_

#include "debug.h"

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
}ControlPin_t;

#endif /* USER_INTERFACES_CONTROL_PIN_H_ */
