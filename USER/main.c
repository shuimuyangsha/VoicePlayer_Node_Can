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
�����ֶ���  ���λ 0 ����ظ�  1 ��Ҫ�ظ�
0x01   ����������
0x02   ��ǰ״̬����
0x03   �汾��Ϣ
0x04   ����״̬

0x0F   �ظ�����
***********************************************/
#define SENSOR_DATA      0x01   //����������  
#define SIGN_DATA        0x02   //״̬����
#define VERSION_DATA     0x03   //�汾��Ϣ
#define ERROR            0x04   //������Ϣ
#define REPLY_DATA       0xF0   //�ظ�����
#define DATA_WAIT_TIME   1000   //���ݻظ��ȴ�ʱ��
/*************************************************
������:
1. ���ջ��������
2. �ȴ����ݻظ����������
3. �ȴ����ݳ�ʱ
4. ��Ч����
5. ���ݻ�ȡ��ʱ
**************************************************/
u8 error_sign = 0;                    //�����־
u8 error_receive_cache_overflow = 0;  //���ջ��������
u8 error_query_cache_overflow = 0;    //�ȴ��ظ����������
u8 error_query_overtime = 0;          //�ȴ����ݳ�ʱ
u8 error_invalid_data = 0;            //��Ч����
u8 error_get_data_overtime = 0;       //���ݻ�ȡ��ʱ
//�ȴ��ظ����� ����ṹ��
#pragma pack (1)           		        //���ݶ���
struct Can1_Reply_Frame    		        //�������ݽṹ�� �ظ�����ʹ��
{
	u8 data_buf[8];	
	u32 time_Stamp;     			          //ʱ���
};
#pragma pack ()
#define SEND_DATA_WAIT_TIME  1000 
/*************************************************/
//#define USART2_DATA_WAIT_TIME   2000   //���ݻظ��ȴ�ʱ��

                          
//��������
void Can1_Reply_Data_Init(struct Can1_Reply_Frame *data);   //��ʼ���ظ�����
void Can1_Send_Data(u8 *data,u8 Funco,u8 number,u16 Data,u16 Data1,u8 sign);    //�������ݴ��
u16 Ger_Send_Data_Id(u8 number);//��ȡ��������ID

char folder01[] = { "/����*/���*/��Ƶ*/" };         //����·���� ��Ŀ¼->���������ļ�->���->��Ƶ ���ļ���Ϊ100����Ƶ�ļ�


char file_name_num000[] = { "/����*/���*/��Ƶ*/000" };       //�������ͣ��뼰ʱ���  
char file_name_num010[] = { "/����*/���*/��Ƶ*/010" };       //��ǰ����Լ5% 
char file_name_num020[] = { "/����*/���*/��Ƶ*/020" };       //��ǰ����Լ10% 
char file_name_num030[] = { "/����*/���*/��Ƶ*/030" };       //��ǰ����Լ15% 
char file_name_num040[] = { "/����*/���*/��Ƶ*/040" };       //��ǰ����Լ20% 
char file_name_num050[] = { "/����*/���*/��Ƶ*/050" };       //��ǰ����Լ25% 
char file_name_num060[] = { "/����*/���*/��Ƶ*/060" };       //��ǰ����Լ30% 
char file_name_num070[] = { "/����*/���*/��Ƶ*/070" };       //��ǰ����Լ35% 
char file_name_num080[] = { "/����*/���*/��Ƶ*/080" };       //��ǰ����Լ40% 
char file_name_num090[] = { "/����*/���*/��Ƶ*/090" };       //��ǰ����Լ45% 
char file_name_num100[] = { "/����*/���*/��Ƶ*/100" };       //��ǰ����Լ50% 
char file_name_num110[] = { "/����*/���*/��Ƶ*/110" };       //��ǰ����Լ55% 
char file_name_num120[] = { "/����*/���*/��Ƶ*/120" };       //��ǰ����Լ60% 
char file_name_num130[] = { "/����*/���*/��Ƶ*/130" };       //��ǰ����Լ65% 
char file_name_num140[] = { "/����*/���*/��Ƶ*/140" };       //��ǰ����Լ70% 
char file_name_num150[] = { "/����*/���*/��Ƶ*/150" };       //��ǰ����Լ75% 
char file_name_num160[] = { "/����*/���*/��Ƶ*/160" };       //��ǰ����Լ80% 
char file_name_num170[] = { "/����*/���*/��Ƶ*/170" };       //��ǰ����Լ85% 
char file_name_num180[] = { "/����*/���*/��Ƶ*/180" };       //��ǰ����Լ90% 
char file_name_num190[] = { "/����*/���*/��Ƶ*/190" };       //��ǰ����Լ95% 
char file_name_num200[] = { "/����*/���*/��Ƶ*/200" };       //��ǰ����Լ100% 



int main(void)
{
	  //char Buffer1[] = {"/����*/���*/��Ƶ*/000"};         //����·���� ��Ŀ¼->���������ļ�->���->��Ƶ ���ļ���Ϊ100����Ƶ�ļ�
	  //char Buffer1[] = { "/����*/���*/��Ƶ*/000" };         //����·���� ��Ŀ¼->���������ļ�->���->��Ƶ ���ļ���Ϊ100����Ƶ�ļ�


//    char Buffer2[] = {"/00001"};    
//    uint8_t Buffer3[3] = {1,2,3};     
	
	int i;
	u8 *pFrame;                                                                   //�����������ָ��
	u8 *pFrame_Reply;                                                             //����ظ���������ָ��
	u32 time = 0;                                                                 //������ݻظ�ʱ��
  u16 send_data_id = 0;                                                         //��������ID
	static u8 error_delay_times = 0;                                              //�ȴ����ݻظ���ʱ�����������
	u8 error_data = 0;                                                            //����״̬

  static u16 enable_sign = 1;                                                   //��ǰ״̬��־ 
  u8 can1_send_data_buf[8] = {0};                                               //���ݷ�������	
	
	struct Can1_Reply_Frame can1_reply_data;                                      //����ṹ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                          			//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	   	
	
  
	TIM4_Init(1000-1,72-1);                                                       //1000us���һ��  	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);    			//CAN����ģʽ��ʼ��, ������500Kbps  
	//IWDG_Init(4,625);    
	Can1_Reply_Data_Init(&can1_reply_data);                                       //��ʼ���ṹ��(���ݻظ�)

//	u32 time = 0;
                                       //��ʼ���ṹ��(���ݻظ�)
	Init_Usart2(9600);
	
    delay_ms(500);              //�ȴ�ģ���ȶ�
  
    JQ8x00_Command_Data(SetVolume,15);         //��������
    delay_ms(10);                               //��������ָ�����һ������ʱ�ȴ�ģ�鴦�������һ��ָ��  

//    JQ8x00_Command_Data(AppointTrack,2);      //������Ŀ2
//    JQ8x00_ZuHeBoFang(Buffer3,3);             //��ZH�ļ���������Ϊ01 02 03��3����Ƶ�ļ����β���
//    delay_ms(10);
	JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num000);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	delay_ms(1500);
	delay_ms(1500);
	delay_ms(1500);
 //   JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num010);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num020);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num030);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num040);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num050);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num060);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num070);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�

	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num080);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num090);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num100);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num110);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num120);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num130);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num140);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num150);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�

	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num160);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num170);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num180);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num190);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�
	//delay_ms(1500);
	//delay_ms(1500);
	//delay_ms(1500);
	//JQ8x00_RandomPathPlay(JQ8X00_SD, file_name_num200);      //���� ��Ŀ¼->����1->����2 ���ļ���Ϊ00002����Ƶ�ļ�



	while(1)
	{
/*****************************������յ�������*****************************************************************************************************/		
		if(1 == Can1_Receive_Data_Sign)                                        			//�յ��µ�����
		{
			for(i=0;i<Can1_data_number;i++)                                      			//ȡ��ȫ������
			{
				if(Set_Can1_Data_Flag(i,Can1_Receive_Data_Buf))                    			//�ж���������  ������ݱ�־
				{
					pFrame = Get_Can1_Recv_Data_Pointer(i,Can1_Receive_Data_Buf);    			//��ȡ����������ָ��
					if((0x55 == *(pFrame + 11)) && (CAN_HARDWARE_NUMBER == *(pFrame + 12))&& (0xFF == *(pFrame + 18)))                                           //֡ͷ  ֡β  �豸���
					{
/************************************����ظ�����********************************************************************************************/
						if(REPLY_DATA == ((*(pFrame + 13)) & REPLY_DATA))                    //������Ϊ�ظ�����
						{
							for(i=0;i<Can1_Reply_data_number;i++)                              //�Ա�������Ҫ�ظ�������
							{
								if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))      //����Ҫ�ظ�������
			          {
									pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//��ȡ����ָ��
									if( (*(pFrame + 11) == *(pFrame_Reply + 0)) && 
											(*(pFrame + 12) == *(pFrame_Reply + 1)) && 
											((*(pFrame + 13) & 0x0F) == (*(pFrame_Reply + 2) & 0x0F)) && 
											(*(pFrame + 14) == *(pFrame_Reply + 3)) && 
											(*(pFrame + 15) == *(pFrame_Reply + 4)) && 
											(*(pFrame + 16) == *(pFrame_Reply + 5)) && 
											(*(pFrame + 17) == *(pFrame_Reply + 6)) && 
											(*(pFrame + 18) == *(pFrame_Reply + 7))   )                 //������ͬ  
									{
										*(pFrame_Reply - 1) = 0;                                      //�����־
									}									
								}
							}
						}
/****************************************����ָ������**************************************************************************************/
						else                                                                  //��������ָ��
						{
							switch ((*(pFrame + 13)) & 0x0F)                                    //�жϹ�����
							{
								case 0x00:                                                   			//ʧ��
//									enable_sign = 0;                                                //���״̬    						
								break;
								case 0x01:                                                   			//ʹ��
//								  enable_sign = 1;    		                                        //���״̬
								break;
								case 0x02:                                                   			 //��ѯ״̬
                  Can1_Send_Data(can1_send_data_buf,SIGN_DATA,(*(pFrame + 12)),enable_sign,0,0);	  //�������
									if(0x80 == (can1_send_data_buf[2] & 0x80))
									{
										pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //��ѯ�洢λ��  ���������Ч
										can1_reply_data.time_Stamp = cnt_time;                        //��¼���ݷ���ʱ��
										for(i=0;i<8;i++)
										{
											can1_reply_data.data_buf[i] = can1_send_data_buf[i];
										}
										memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //��������//���ݴ�����뻺�����ȴ��յ��ظ����ݺ�����									
									}
									send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));              //��ȡ����ID
									Can_Send_Msg(can1_send_data_buf,8,send_data_id);                //�ϴ�״̬
								break;
								case 0x03:                                                        //��ѯ�汾��Ϣ    
                  Can1_Send_Data(can1_send_data_buf,VERSION_DATA,(*(pFrame + 12)),g_configParamDefault.HardwareVersionInformation,
								                                                g_configParamDefault.SoftwareVersionInformation,0);	    //�������
									if(0x80 == (can1_send_data_buf[2] & 0x80))
									{
										pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //��ѯ�洢λ��  ���������Ч
										can1_reply_data.time_Stamp = cnt_time;                        //��¼���ݷ���ʱ��
										for(i=0;i<8;i++)
										{
											can1_reply_data.data_buf[i] = can1_send_data_buf[i];
										}
										memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //��������//���ݴ�����뻺�����ȴ��յ��ظ����ݺ�����									
									}
									send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));              //��ȡ����ID
									Can_Send_Msg(can1_send_data_buf,8,send_data_id);                //�ϴ�״̬
								break;
			
								default : error_invalid_data = 1;                                                          //��Чָ��
									        error_sign = 1;                                                                 //��ǲ�������
							}		
/**********************������Ҫ�ظ�������****************************************************************************************************************/
							if(0x80 == (*(pFrame + 13) & 0xF0))                              			//���ݻظ� ����
							{
								*(pFrame + 13) =((*(pFrame + 13)) | 0xF0);                          //�ظ����� ������ ��4λΪ1111
								send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));                  //��ȡ����ID
								Can_Send_Msg((pFrame + 11),8,send_data_id);                  		    //��Ҫ�ظ������ݴ������ظ�����(����ԭ�ⲻ�����ع����� �ı�)
							}	
/***************************end***************************************************************************************************/				
						}
					}
				}
				else
				{
					break;                                                           			//���ݴ������			
				}
			}
			Can1_Receive_Data_Sign = 0;                                          			//������ݴ������
		}
/**************************************************��صȴ��ظ�����**********************************************************************************************************/
		for(i=0;i<Can1_Reply_data_number;i++)                                		    //�鿴�ȴ��ظ������Ƿ�������  ��������
		{
			if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))           		//����Ҫ�ظ�������
			{
				pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//��ȡ����ָ��
				time = (*(pFrame_Reply + 11) << 24) | (*(pFrame_Reply + 10) << 16) | (*(pFrame_Reply + 9) << 8) | (*(pFrame_Reply + 8));  //��ȡ����ʱ��
				if((cnt_time - time) > DATA_WAIT_TIME)                                          //�ȴ�ʱ�䳬ʱ
				{
					send_data_id = Ger_Send_Data_Id((*(pFrame + 12)));                            //��ȡ����ID
					Can_Send_Msg(pFrame_Reply,8,send_data_id);																    //���·���ָ��
					*((int*)(pFrame_Reply + 8)) = cnt_time;																        //����ʱ��
					if(error_delay_times >= 5)
					{
						error_query_overtime = 1;                                                   //������ݵȴ���ʱ
						error_sign = 1;                                                             //��Ǵ������
						error_delay_times = 0;                                                      //��ռ���
					}
					error_delay_times++;                                                          //��ʱ�����ۼ�
				}
			}
		}				
/**************************************************�ϴ�����������*********************************************************************************************************/
		if((1 == can_send_data_sing))            //���ݷ���
		{
			if(1 == enable_sign)           //ʹ��  
			{
				if((cnt_time - usart2_send_data_time) > SEND_DATA_WAIT_TIME)            //���ݲ�����ʱ
				{
					Can1_Send_Data(can1_send_data_buf,SENSOR_DATA,CAN_HARDWARE_NUMBER,0xFFFF,0,0); //���ݴ��  ���������Ч
					if(0x80 == (can1_send_data_buf[2] & 0x80))
					{
						pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //��ѯ�洢λ��  ���������Ч
						can1_reply_data.time_Stamp = cnt_time;                        //��¼���ݷ���ʱ��
						for(i=0;i<8;i++)
						{
							can1_reply_data.data_buf[i] = can1_send_data_buf[i];
						}
						memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //��������//���ݴ�����뻺�����ȴ��յ��ظ����ݺ�����									
					}
					send_data_id = Ger_Send_Data_Id(CAN_HARDWARE_NUMBER);                        //��ȡ����ID
					Can_Send_Msg(can1_send_data_buf,8,send_data_id);                                     //��������										
				}
				else
				{
					Can1_Send_Data(can1_send_data_buf,SENSOR_DATA,CAN_HARDWARE_NUMBER,second_ultrasonic_data/10,0,0); //���ݴ��  ���������Ч
					if(0x80 == (can1_send_data_buf[2] & 0x80))
					{
						pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //��ѯ�洢λ��  ���������Ч
						can1_reply_data.time_Stamp = cnt_time;                        //��¼���ݷ���ʱ��
						for(i=0;i<8;i++)
						{
							can1_reply_data.data_buf[i] = can1_send_data_buf[i];
						}
						memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //��������//���ݴ�����뻺�����ȴ��յ��ظ����ݺ�����									
					}
					send_data_id = Ger_Send_Data_Id(CAN_HARDWARE_NUMBER);                                //��ȡ����ID
					Can_Send_Msg(can1_send_data_buf,8,send_data_id);                                     //��������							
				}
			}
      can_send_data_sing = 0;//�������   �ȴ���һ�·���                                           
		}
/**********************************���ʹ�����Ϣ******************************************************************************************************************/		
//		if(1 == error_sign)                  //�д�����
//		{
//			error_data = ((((((((error_data | error_receive_cache_overflow) << 1) | 
//																				  error_query_cache_overflow) << 1) | 
//																							  error_query_overtime) << 1) | 
//																							    error_invalid_data) << 1) |
//                                        			error_get_data_overtime;
//			Can1_Send_Data(can1_send_data_buf,ERROR,CAN1_HARDWARE_NUMBER,error_data,0,1);	  //�������
//			if(0x80 == (can1_send_data_buf[2] & 0x80))
//			{
//				for(i=0;i<Can1_Reply_data_number;i++)                              //�Ա�������Ҫ�ظ�������   ����Ƿ����ظ�����ָ��
//				{
//					if(Set_Can1_Reply_Data_Flag(i,Can1_Reply_Receive_data_buf))      //����Ҫ�ظ�������
//					{
//						pFrame_Reply = Get_Can1_Reply_Data_Pointer(i,Can1_Reply_Receive_data_buf);		//��ȡ����ָ��
//						if( (*(can1_send_data_buf + 0) == *(pFrame_Reply + 0)) && 
//								(*(can1_send_data_buf + 1) == *(pFrame_Reply + 1)) && 
//								((*(can1_send_data_buf + 2) & 0x0F) == (*(pFrame_Reply + 2) & 0x0F)) && 
//								(*(can1_send_data_buf + 3) == *(pFrame_Reply + 3)) && 
//								(*(can1_send_data_buf + 4) == *(pFrame_Reply + 4)) && 
//								(*(can1_send_data_buf + 5) == *(pFrame_Reply + 5)) && 
//								(*(can1_send_data_buf + 6) == *(pFrame_Reply + 6)) && 
//								(*(can1_send_data_buf + 7) == *(pFrame_Reply + 7))   )                 //������ͬ  
//						{
//					    goto end;
//						}									
//					}
//				}	
//				pFrame_Reply = Get_Can1_Reply_Pointer(Can1_Reply_Receive_data_buf); //��ѯ�洢λ��  ���������Ч
//				can1_reply_data.time_Stamp = cnt_time;                        //��¼���ݷ���ʱ��
//				for(i=0;i<8;i++)
//				{
//					can1_reply_data.data_buf[i] = can1_send_data_buf[i];
//				}
//				memcpy((void*)(pFrame_Reply + 1), (const void*)(&can1_reply_data), sizeof(struct Can1_Reply_Frame));   //��������//���ݴ�����뻺�����ȴ��յ��ظ����ݺ�����								
//			}
//			send_data_id = Ger_Send_Data_Id(CAN1_HARDWARE_NUMBER);                            //��ȡ����ID
//			Can_Send_Msg(can1_send_data_buf,8,send_data_id);                        	    		//�ϴ�״̬						
//			end:;			
//			error_data = 0;                    //���������
//			error_receive_cache_overflow = 0;  //���ջ��������
//			error_query_cache_overflow = 0;    //�ȴ��ظ����������
//			error_query_overtime = 0;          //�ȴ����ݳ�ʱ
//			error_invalid_data = 0;            //��Ч����
//			error_sign = 0;
//		}
/***********************************************************end*************************************************************************************/		
		IWDG_Feed();                                                                //ι��  
	}
}

/**********************************************************************
�������� : void Can1_Reply_Data_Init(struct Can1_Reply_Frame *data)
�������� : ��ʼ���ṹ�����
������� : 
�� �� ֵ :  ��
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
�������� : void Can1_Send_Data(u8 *data,u8 Funco,u8 number,u16 Data,u16 Data1,u8 sign)
�������� : ���ݴ������
������� : 
�� �� ֵ :  ��
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
�������� : u16 Ger_Send_Data_Id(u8 number)
�������� : ��ȡ��������ID
������� : 
�� �� ֵ :  ��
***********************************************************************/
u16 Ger_Send_Data_Id(u8 number)
{
	u16 send_id = 0x0005;   //��ʼ���ڴ���ID
	switch (number)
	{
		case ULTRASONUND_1_NUMBER:     //����1
			send_id = 0x0015;
		break;
		case ULTRASONUND_2_NUMBER:     //����2
			send_id = 0x0025;
		break;
		case ULTRASONUND_3_NUMBER:     //����3
			send_id = 0x0035;
		break;
		case ULTRASONUND_4_NUMBER:     //����4
			send_id = 0x0045;
		break;
		case ULTRASONUND_5_NUMBER:     //����5
			send_id = 0x0055;
		break;
		case ULTRASONUND_6_NUMBER:     //����6
			send_id = 0x0065;
		break;
		case ULTRASONUND_7_NUMBER:     //����7
			send_id = 0x0075;
		break;
		case ULTRASONUND_8_NUMBER:     //����8
			send_id = 0x0085;
		break;
		case ULTRASONUND_9_NUMBER:     //����1
			send_id = 0x0095;
		break;
		case ULTRASONUND_10_NUMBER:     //����2
			send_id = 0x00A5;
		break;
		case ULTRASONUND_11_NUMBER:     //����3
			send_id = 0x00B5;
		break;
		case ULTRASONUND_12_NUMBER:     //����4
			send_id = 0x00C5;
		break;
		case ULTRASONUND_13_NUMBER:     //����5
			send_id = 0x00D5;
		break;
		case ULTRASONUND_14_NUMBER:     //����6
			send_id = 0x00E5;
		break;
		case ULTRASONUND_15_NUMBER:     //����7
			send_id = 0x00F5;
		break;
		case ULTRASONUND_16_NUMBER:     //����8
			send_id = 0x0105;
		break;
		default :
			send_id = 0x0005;             //��������  ��Ǵ���ID
	}
	return send_id;
}

