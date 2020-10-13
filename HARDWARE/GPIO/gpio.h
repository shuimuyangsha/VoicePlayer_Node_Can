#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"

void Gpio_Init(void);

void Close_Power(void);
void Open_Power(void);
u8 Get_Power_State(void);
#endif
