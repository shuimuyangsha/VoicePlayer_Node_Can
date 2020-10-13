#ifndef __TIME_H
#define __TIME_H
#include "sys.h"



extern u32 cnt_time;
extern u8 can_send_data_sing;

void TIM4_Init(u16 arr,u16 psc);



#endif

