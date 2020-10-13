#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	       1		//0,不使能;1,使能.		
#define Can1_Data_Lengh            20   //can1 接收数据长度
#define Can1_data_number           20   //can1 缓存区存储数据上限
#define Can1_Reply_Data_Lengh      12   //等待回复数据数据长度
#define Can1_Reply_data_number     20   //等待回复数据缓存区存储数据上限

extern u8 Can1_Receive_Data_Sign;       //接收数据标志
extern u8 Can1_Receive_Data_Buf[(Can1_Data_Lengh +1) * Can1_data_number];  //接收缓存区
extern u8 Can1_Reply_Receive_data_buf[(Can1_Reply_Data_Lengh + 1)*Can1_Reply_data_number];//等待回复数据存储数组

						    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);  //CAN初始化
u8 Can_Send_Msg(u8* msg,u8 len ,u16 id);						                //发送数据

int Set_Can1_Data_Flag(int i,u8* p);
u8* Get_Can1_Recv_Data_Pointer(int i ,u8* p);



u8* Get_Can1_Reply_Pointer(u8* p);
u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p);
int Set_Can1_Reply_Data_Flag(int i,u8* p);

#endif

