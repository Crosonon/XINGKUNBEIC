#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "stm32f1xx_hal.h"
#include "Coordinate.h"


extern Control_Mode Current_Mode;

void Control_Init(void);
uint8_t Control_SetMode(Control_Mode Set_Mode);
uint8_t Control_EndMode(void);


#endif
