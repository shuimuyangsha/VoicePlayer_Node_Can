#ifndef __USART2_H
#define __USART2_H
	
#include "sys.h" 


extern u16 second_ultrasonic_data;       //接收到的数据  
extern u32 usart2_send_data_time;        //接收数据的时间
//extern u8 usart2_new_data_sign;          //窗口接收到新数据标志

void Init_Usart2(u32 bound);
void usart2_send_data(void);

#endif


