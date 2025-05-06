#ifndef __BEEP_H__
#define __BEEP_H__

#include "stm32f1xx_hal.h"
#include "main.h"

extern uint32_t time;
extern uint32_t Beep_time;

void Beep_Request(float t);
void Beep_Update(void);

#endif
