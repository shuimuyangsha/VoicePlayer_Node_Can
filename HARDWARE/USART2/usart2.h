#ifndef __USART2_H
#define __USART2_H
	
#include "sys.h" 


extern u16 second_ultrasonic_data;       //���յ�������  
extern u32 usart2_send_data_time;        //�������ݵ�ʱ��
//extern u8 usart2_new_data_sign;          //���ڽ��յ������ݱ�־

void Init_Usart2(u32 bound);
void usart2_send_data(void);

#endif


