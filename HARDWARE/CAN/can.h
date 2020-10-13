#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	       1		//0,��ʹ��;1,ʹ��.		
#define Can1_Data_Lengh            20   //can1 �������ݳ���
#define Can1_data_number           20   //can1 �������洢��������
#define Can1_Reply_Data_Lengh      12   //�ȴ��ظ��������ݳ���
#define Can1_Reply_data_number     20   //�ȴ��ظ����ݻ������洢��������

extern u8 Can1_Receive_Data_Sign;       //�������ݱ�־
extern u8 Can1_Receive_Data_Buf[(Can1_Data_Lengh +1) * Can1_data_number];  //���ջ�����
extern u8 Can1_Reply_Receive_data_buf[(Can1_Reply_Data_Lengh + 1)*Can1_Reply_data_number];//�ȴ��ظ����ݴ洢����

						    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);  //CAN��ʼ��
u8 Can_Send_Msg(u8* msg,u8 len ,u16 id);						                //��������

int Set_Can1_Data_Flag(int i,u8* p);
u8* Get_Can1_Recv_Data_Pointer(int i ,u8* p);



u8* Get_Can1_Reply_Pointer(u8* p);
u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p);
int Set_Can1_Reply_Data_Flag(int i,u8* p);

#endif

