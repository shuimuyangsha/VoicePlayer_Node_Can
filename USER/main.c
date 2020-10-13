#include "delay.h"
#include "sys.h"
#include "gpio.h"
#include "can.h"
#include "time.h"
#include "iwdg.h"
#include "VersionInformation.h"
#include "commonedef.h"
#include <string.h>
#include "usart2.h"
#include "led.h"
#include "JQ8X00.h"

/***********************************************
功能字定义  最高位 0 无需回复  1 需要回复
0x01   传感器数据
0x02   当前状态数据
0x03   版本信息
0x04   错误状态

0x0F   回复数据
***********************************************/
#define SENSOR_DATA      0x01   //传感器数据  
#define SIGN_DATA        0x02   //状态数据
#define VERSION_DATA     0x03   //版本信息
#define ERROR            0x04   //错误信息
#define REPLY_DATA       0xF0   //回复数据
#define DATA_WAIT_TIME   1000   //数据回复等待时间
/*************************************************
错误定义:
1. 接收缓存区溢出
2. 等待数据回复缓存区溢出
3. 等待数据超时
4. 无效数据
5. 数据获取超时
**************************************************/
u8 error_sign = 0;                    //错误标志
u8 error_receive_cache_overflow = 0;  //接收缓存区溢出
u8 error_query_cache_overflow = 0;    //等待回复缓存区溢出
u8 error_query_overtime = 0;          //等待数据超时
u8 error_invalid_data = 0;            //无效数据
u8 error_get_data_overtime = 0;       //数据获取超时
//等待回复数据 打包结构体
#pragma pack (1)           		        //数据对齐
struct Can1_Reply_Frame    		        //定义数据结构体 回复数据使用
{
	u8 data_buf[8];	
	u32 time_Stamp;     			          //时间戳
};
#pragma pack ()
#define SEND_DATA_WAIT_TIME  1000 
/*************************************************/
//#define USART2_DATA_WAIT_TIME   2000   //数据回复等待时间

                          
//函数声明
void Can1_Reply_Data_Init(struct Can1_Reply_Frame *data);   //初始化回复数据
void Can1_Send_Data(u8 *data,u8 Funco,u8 number,u16 Data,u16 Data1,u8 sign);    //发送数据打包
u16 Ger_Send_Data_Id(u8 number);//获取发送数据ID

char folder01[] = { "/语音*/电池*/音频*/" };         //播放路径： 根目录->语音播报文件->电池->音频 下文件名为100的音频文件


char file_name_num000[] = { "/语音*/电池*/音频*/000" };       //电量过低，请及时充电  
char file_name_num010[] = { "/语音*/电池*/音频*/010" };       //当前电量约5% 
char file_name_num020[] = { "/语音*/电池*/音频*/020" };       //当前电量约10% 
char file_name_num030[] = { "/语音*/电池*/音频*/030" };       //当前电量约15% 
char file_name_num040[] = { "/语音*/电池*/音频*/040" };       //当前电量约20% 
char file_name_num050[] = { "/语音*/电池*/音频*/050" };       //当前电量约25% 
char file_name_num060[] = { "/语音*/电池*/音频*/060" };       //当前电量约30% 
char file_name_num070[] = { "/语音*/电池*/音频*/070" };       //当前电量约35% 
char file_name_num080[] = { "/语音*/电池*/音频*/080" };       //当前电量约40% 
char file_name_num090[] = { "/语音*/电池*/音频*/090" };       //当前电量约45% 
char file_name_num100[] = { "/语音*/电池*/音频*/100" };       //当前电量约50% 
char file_name_num110[] = { "/语音*/电池*/音频*/110" };       //当前电量约55% 
char file_name_num120[] = { "/语音*/电池*/音频*/120" };       //当前电量约60% 
char file_name_num130[] = { "/语音*/电池*/音频*/130" };       //当前电量约65% 
char file_name_num140[] = { "/语音*/电池*/音频*/140" };       //当前电量约70% 
char file_name_num150[] = { "/语音*/电池*/音频*/150" };       //当前电量约75% 
char file_name_num160[] = { "/语音*/电池*/音频*/160" };       //当前电量约80% 
char file_name_num170[] = { "/语音*/电池*/音频*/170" };       //当前电量约85% 
char file_name_num180[] = { "/语音*/电池*/音频*/180" };       //当前电量约90% 
char file_name_num190[] = { "/语音*/电池*/音频*/190" };       //当前电量约95% 
char file_name_num200[] = { "/语音*/电池*/音频*/200" };       //当前电量约100% 



int main(void)
{
	  //char Buffer1[] = {"/语音*/电池*/音频*/000"};         //播放路径： 根目录->语音播报文件->电池->音频 下文件名为100的音频文件
	  //char Buffer1[] = { "/语音*/电池*/音频*/000" };         //播放路径： 根目录->语音播报文件->电池->音频 下文件名为100的音频文件


//    char Buffer2[] = {"/00001"};    
//    uint8_t Buffer3[3] = {1,2,3};     
	
	int i;
	u8 *pFrame;                                                                   //处理接收数据指针
	u8 *pFrame_Reply;                                                             //处理回复数据数据指针
	u32 time = 0;                                                                 //监控数据回复时间
  u16 send_data_id = 0;                                                         //发送数据ID
	static u8 error_delay_times = 0;                                              //等待数据回复超时错误计数变量
	u8 error_data = 0;                                                            //错误状态

  static u16 enable_sign = 1;                                                   //当前状态标志 
  u8 can1_send_data_buf[8] = {0};                                               //数据发送数组	
	
	struct Can1_Reply_Frame can1_reply_data;                                      //定义结构体变量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          			//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	   	
	
  
	TIM4_Init(1000-1,72-1);                                                       //1000us溢出一次  	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);    			//CAN正常模式初始化, 波特率500Kbps  
	//IWDG_Init(4,625);    
	Can1_Reply_Data_Init(&can1_reply_data);                                       //初始化结构体(数据回复)

//	u32 time = 0;
                                       //初始化结构体(数据回复)
	Init_Usart2(9600);
	
    delay_ms(500);              //等待模块稳定
  
    JQ8x00_Command_Data(SetVolume,15);         //设置音量
    delay_ms(10);                               //连续发送指令，加入一定的延时等待模块处理完成上一条指令  

//    JQ8x00_Command_Data(AppointTrack,2);      //播放曲目2
//    JQ8x00_ZuHeBoFang(Buffer3,3);             //将ZH文件夹下名字为01 02 03的3个音频文件依次播放
//    delay_ms(10);
	JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num000);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	delay_ms(1500);
	delay_ms(1500);
	delay_ms(1500);
 //   JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num010);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num020);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num030);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num040);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num050);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num060);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num070);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件

	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num080);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num090);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num100);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num110);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num120);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num130);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num140);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num150);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件

	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num160);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num170);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num180);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num190);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num200);      //播放 根目录->测试1->测试2 下文件名为00002的音频文件



	while(1)
	{
/*****************************处理接收到的数据*****************************************************************************************************/		
		if(1 == Can1_Receive_Data_Sign)                                        			//收到新的数据
		{
			for(i=0;i<Can1_data_number;i++)                                      			//取出全部数据
			{
				if(Set_Can1_Data_Flag(i,Can1_Receive_Data_Buf))                    			//判断有无数据  清楚数据标志
				{
					pFrame = Get_Can1_Recv_Data_Pointer(i,Can1_Receive_Data_Buf);    			//读取缓存区数据指针
					if((0x55 == *(pFrame + 11)) && (CAN_HARDWARE_NUMBER == *(pFrame + 12))&& (0xFF == *(pFrame + 18)))                                           //帧头  帧尾  设备编号
					{
/************************************处理回复数据********************************************************************************************/
						if(REPLY_DATA == ((*(pFrame + 13)) & REPLY_DATA))                    //次数据为回复数据
						{
							for(i=0;i<Can1_Reply_data_number;i++)                              //对比所有需要回复的数据
							{
								if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))      //有需要回复的数据
			          {
									pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//获取数据指针
									if( (*(pFrame + 11) == *(pFrame_Reply + 0)) && 
											(*(pFrame + 12) == *(pFrame_Reply + 1)) && 
											((*(pFrame + 13) & 0x0F) == (*(pFrame_Reply + 2) & 0x0F)) && 
											(*(pFrame + 14) == *(pFrame_Reply + 3)) && 
											(*(pFrame + 15) == *(pFrame_Reply + 4)) && 
											(*(pFrame + 16) == *(pFrame_Reply + 5)) && 
											(*(pFrame + 17) == *(pFrame_Reply + 6)) && 
											(*(pFrame + 18) == *(pFrame_Reply + 7))   )                 //数据相同  
									{
										*(pFrame_Reply - 1) = 0;                                      //清除标志
									}									
								}
							}
						}
/****************************************处理指令数据**************************************************************************************/
						else                                                                  //正常控制指令
						{
							switch ((*(pFrame + 13)) & 0x0F)                                    //判断功能字
							{
								case 0x00:                                                   			//失能
//									enable_sign = 0;                                                //标记状态    						
								break;
								case 0x01:                                                   			//使能
//								  enable_sign = 1;    		                                        //标记状态
								break;
								case 0x02:                                                   			 //查询状态
                  Can1_Send_Data(can1_send_data_buf,SIGN_DATA,(*(pFrame + 12)),enable_sign,0,0);	  //打包数据
									if(0x80 == (can1_send_data_buf[2] & 0x80))
									{
										pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //查询存储位置  标记数据有效
										can1_reply_data.time_Stamp = cnt_time;                        //记录数据发送时间
										for(i=0;i<8;i++)
										{
											can1_reply_data.data_buf[i] = can1_send_data_buf[i];
										}
										memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //复制数组//数据打包放入缓存区等待收到回复数据后消除									
									}
									send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));              //获取发送ID
									Can_Send_Msg(can1_send_data_buf,8,send_data_id);                //上传状态
								break;
								case 0x03:                                                        //查询版本信息    
                  Can1_Send_Data(can1_send_data_buf,VERSION_DATA,(*(pFrame + 12)),g_configParamDefault.HardwareVersionInformation,
								                                                g_configParamDefault.SoftwareVersionInformation,0);	    //打包数据
									if(0x80 == (can1_send_data_buf[2] & 0x80))
									{
										pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //查询存储位置  标记数据有效
										can1_reply_data.time_Stamp = cnt_time;                        //记录数据发送时间
										for(i=0;i<8;i++)
										{
											can1_reply_data.data_buf[i] = can1_send_data_buf[i];
										}
										memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //复制数组//数据打包放入缓存区等待收到回复数据后消除									
									}
									send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));              //获取发送ID
									Can_Send_Msg(can1_send_data_buf,8,send_data_id);                //上传状态
								break;
			
								default : error_invalid_data = 1;                                                          //无效指令
									        error_sign = 1;                                                                 //标记产生错误
							}		
/**********************处理需要回复的数据****************************************************************************************************************/
							if(0x80 == (*(pFrame + 13) & 0xF0))                              			//数据回复 需求
							{
								*(pFrame + 13) =((*(pFrame + 13)) | 0xF0);                          //回复数据 功能字 高4位为1111
								send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));                  //获取发送ID
								Can_Send_Msg((pFrame + 11),8,send_data_id);                  		    //需要回复的数据打包进入回复数组(数据原封不动返回功能字 改变)
							}	
/***************************end***************************************************************************************************/				
						}
					}
				}
				else
				{
					break;                                                           			//数据处理完成			
				}
			}
			Can1_Receive_Data_Sign = 0;                                          			//标记数据处理完成
		}
/**************************************************监控等待回复数据**********************************************************************************************************/
		for(i=0;i<Can1_Reply_data_number;i++)                                		    //查看等待回复数据是否有数据  所有数据
		{
			if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))           		//有需要回复的数据
			{
				pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//获取数据指针
				time = (*(pFrame_Reply + 11) << 24) | (*(pFrame_Reply + 10) << 16) | (*(pFrame_Reply + 9) << 8) | (*(pFrame_Reply + 8));  //获取发送时间
				if((cnt_time - time) > DATA_WAIT_TIME)                                          //等待时间超时
				{
					send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));                            //获取发送ID
					Can_Send_Msg(pFrame_Reply,8,send_data_id);																    //从新发送指令
					*((int*)(pFrame_Reply + 8)) = cnt_time;																        //更新时间
					if(error_delay_times >= 5)
					{
						error_query_overtime = 1;                                                   //标记数据等待超时
						error_sign = 1;                                                             //标记错误产生
						error_delay_times = 0;                                                      //清空计数
					}
					error_delay_times++;                                                          //超时计数累加
				}
			}
		}				
/**************************************************上传传感器数据*********************************************************************************************************/
		if((1 == can_send_data_sing))            //数据发送
		{
			if(1 == enable_sign)           //使能  
			{
				if((cnt_time - usart2_send_data_time) > SEND_DATA_WAIT_TIME)            //数据测量超时
				{
					Can1_Send_Data(can1_send_data_buf,SENSOR_DATA,CAN_HARDWARE_NUMBER,0xFFFF,0,0); //数据打包  标记数据无效
					if(0x80 == (can1_send_data_buf[2] & 0x80))
					{
						pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //查询存储位置  标记数据有效
						can1_reply_data.time_Stamp = cnt_time;                        //记录数据发送时间
						for(i=0;i<8;i++)
						{
							can1_reply_data.data_buf[i] = can1_send_data_buf[i];
						}
						memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //复制数组//数据打包放入缓存区等待收到回复数据后消除									
					}
					send_data_id = Ger_Send_Data_Id(CAN_HARDWARE_NUMBER);                        //获取发送ID
					Can_Send_Msg(can1_send_data_buf,8,send_data_id);                                     //发送数据										
				}
				else
				{
					Can1_Send_Data(can1_send_data_buf,SENSOR_DATA,CAN_HARDWARE_NUMBER,second_ultrasonic_data/10,0,0); //数据打包  标记数据无效
					if(0x80 == (can1_send_data_buf[2] & 0x80))
					{
						pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //查询存储位置  标记数据有效
						can1_reply_data.time_Stamp = cnt_time;                        //记录数据发送时间
						for(i=0;i<8;i++)
						{
							can1_reply_data.data_buf[i] = can1_send_data_buf[i];
						}
						memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //复制数组//数据打包放入缓存区等待收到回复数据后消除									
					}
					send_data_id = Ger_Send_Data_Id(CAN_HARDWARE_NUMBER);                                //获取发送ID
					Can_Send_Msg(can1_send_data_buf,8,send_data_id);                                     //发送数据							
				}
			}
      can_send_data_sing = 0;//发送完成   等待下一下发送                                           
		}
/**********************************发送错误信息******************************************************************************************************************/		
//		if(1 == error_sign)                  //有错误发生
//		{
//			error_data = ((((((((error_data | error_receive_cache_overflow) << 1) | 
//																				  error_query_cache_overflow) << 1) | 
//																							  error_query_overtime) << 1) | 
//																							    error_invalid_data) << 1) |
//                                        			error_get_data_overtime;
//			Can1_Send_Data(can1_send_data_buf,ERROR,CAN1_HARDWARE_NUMBER,error_data,0,1);	  //打包数据
//			if(0x80 == (can1_send_data_buf[2] & 0x80))
//			{
//				for(i=0;i<Can1_Reply_data_number;i++)                              //对比所有需要回复的数据   检查是否有重复数据指令
//				{
//					if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))      //有需要回复的数据
//					{
//						pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//获取数据指针
//						if( (*(can1_send_data_buf + 0) == *(pFrame_Reply + 0)) && 
//								(*(can1_send_data_buf + 1) == *(pFrame_Reply + 1)) && 
//								((*(can1_send_data_buf + 2) & 0x0F) == (*(pFrame_Reply + 2) & 0x0F)) && 
//								(*(can1_send_data_buf + 3) == *(pFrame_Reply + 3)) && 
//								(*(can1_send_data_buf + 4) == *(pFrame_Reply + 4)) && 
//								(*(can1_send_data_buf + 5) == *(pFrame_Reply + 5)) && 
//								(*(can1_send_data_buf + 6) == *(pFrame_Reply + 6)) && 
//								(*(can1_send_data_buf + 7) == *(pFrame_Reply + 7))   )                 //数据相同  
//						{
//					    goto end;
//						}									
//					}
//				}	
//				pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //查询存储位置  标记数据有效
//				can1_reply_data.time_Stamp = cnt_time;                        //记录数据发送时间
//				for(i=0;i<8;i++)
//				{
//					can1_reply_data.data_buf[i] = can1_send_data_buf[i];
//				}
//				memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //复制数组//数据打包放入缓存区等待收到回复数据后消除								
//			}
//			send_data_id = Ger_Send_Data_Id(CAN1_HARDWARE_NUMBER);                            //获取发送ID
//			Can_Send_Msg(can1_send_data_buf,8,send_data_id);                        	    		//上传状态						
//			end:;			
//			error_data = 0;                    //清除错误标记
//			error_receive_cache_overflow = 0;  //接收缓存区溢出
//			error_query_cache_overflow = 0;    //等待回复缓存区溢出
//			error_query_overtime = 0;          //等待数据超时
//			error_invalid_data = 0;            //无效数据
//			error_sign = 0;
//		}
/***********************************************************end*************************************************************************************/		
		IWDG_Feed();                                                                //喂狗  
	}
}

/**********************************************************************
函数名称 : void Can1_Reply_Data_Init(struct Can1_Reply_Frame *data)
函数功能 : 初始化结构体参数
输入参数 : 
返 回 值 :  无
***********************************************************************/
void Can1_Reply_Data_Init(struct Can1_Reply_Frame *data)
{
	data->data_buf[0]     		  = 0x55;       
	data->data_buf[1]          	= 0x00;
	data->data_buf[2]       	  = 0x00;
	data->data_buf[3]        	  = 0x00;
	data->data_buf[4]      		  = 0x00;
	data->data_buf[5]        	  = 0x00;
	data->data_buf[6]        		= 0x00;
	data->data_buf[7]     		  = 0xFF;      
  data->time_Stamp       			= 0x0000;
}
/**********************************************************************
函数名称 : void Can1_Send_Data(u8 *data,u8 Funco,u8 number,u16 Data,u16 Data1,u8 sign)
函数功能 : 数据打包函数
输入参数 : 
返 回 值 :  无
***********************************************************************/
void Can1_Send_Data(u8 *data,u8 Funco,u8 number,u16 Data,u16 Data1,u8 sign)
{
	data[0]     		  = 0x55;       
	data[1]          	= number;
	data[2]       	  = Funco;	
	if(sign)
	{
		data[2] = data[2] | 0x80;
	}
	data[3]        	  = Data & 0x00FF;
	data[4]      		  = (Data >> 8) & 0x00FF;
	data[5]        	  = Data1 & 0x00FF;
	data[6]        		= (Data1 >> 8) & 0x00FF;;
	data[7]     		  = 0xFF;  
}
/**********************************************************************
函数名称 : u16 Ger_Send_Data_Id(u8 number)
函数功能 : 获取发送数据ID
输入参数 : 
返 回 值 :  无
***********************************************************************/
u16 Ger_Send_Data_Id(u8 number)
{
	u16 send_id = 0x0005;   //初始等于错误ID
	switch (number)
	{
		case ULTRASONUND_1_NUMBER:     //超声1
			send_id = 0x0015;
		break;
		case ULTRASONUND_2_NUMBER:     //超声2
			send_id = 0x0025;
		break;
		case ULTRASONUND_3_NUMBER:     //超声3
			send_id = 0x0035;
		break;
		case ULTRASONUND_4_NUMBER:     //超声4
			send_id = 0x0045;
		break;
		case ULTRASONUND_5_NUMBER:     //超声5
			send_id = 0x0055;
		break;
		case ULTRASONUND_6_NUMBER:     //超声6
			send_id = 0x0065;
		break;
		case ULTRASONUND_7_NUMBER:     //超声7
			send_id = 0x0075;
		break;
		case ULTRASONUND_8_NUMBER:     //超声8
			send_id = 0x0085;
		break;
		case ULTRASONUND_9_NUMBER:     //超声1
			send_id = 0x0095;
		break;
		case ULTRASONUND_10_NUMBER:     //超声2
			send_id = 0x00A5;
		break;
		case ULTRASONUND_11_NUMBER:     //超声3
			send_id = 0x00B5;
		break;
		case ULTRASONUND_12_NUMBER:     //超声4
			send_id = 0x00C5;
		break;
		case ULTRASONUND_13_NUMBER:     //超声5
			send_id = 0x00D5;
		break;
		case ULTRASONUND_14_NUMBER:     //超声6
			send_id = 0x00E5;
		break;
		case ULTRASONUND_15_NUMBER:     //超声7
			send_id = 0x00F5;
		break;
		case ULTRASONUND_16_NUMBER:     //超声8
			send_id = 0x0105;
		break;
		default :
			send_id = 0x0005;             //错误输入  标记错误ID
	}
	return send_id;
}

