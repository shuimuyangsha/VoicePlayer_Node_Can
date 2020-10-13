#include "can.h"
//#include "gpio.h"
////#include "includes.h"
////#include <stdlib.h>
#include <string.h>

u8 Can1_Receive_Data_Buf[(Can1_Data_Lengh +1) * Can1_data_number] = {0};    //21*20   �������ݻ�����
u8 Can1_Receive_Data_Sign = 0;                                              //������µĿ���ָ��
u8* Get_Can1_Recv_Pointer(u8* p);

u8 Can1_Reply_Receive_data_buf[(Can1_Reply_Data_Lengh + 1)*Can1_Reply_data_number] = {0};  /*�洢��Ҫ�ظ����ݵ�����*/


/*****************************************************************************************************************
CAN��ʼ��
tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
������Ϊ:36M/((8+9+1)*4)=500Kbps
����ֵ:0,��ʼ��OK;
    ����,��ʼ��ʧ��; 
*****************************************************************************************************************/
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		    GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE                                       					 									 //�Ƿ�ʹ���ж�
	NVIC_InitTypeDef  		  NVIC_InitStructure;
#endif
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);     					   									 //ʹ��CAN1ʱ��
#if HARDWARE_VERSIONS_V1_0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     					 									 //ʹ��GOIOBʱ��	

  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);            					  								 //����ӳ��can1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	          					  								 //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			           					      								 //��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	          					    								 //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			           					     									 //��ʼ��IO
#endif
	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			             					       									 //��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;																										 //����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;																										 //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=DISABLE;																										 //��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 																								 //���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=ENABLE;																		 								 //���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	        																						 //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;																												 //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 																											 //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;																				                 //Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        													                 //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        														                 //��ʼ��CAN1 
/*****************************************************************************************************************************************************************/
	CAN_FilterInitStructure.CAN_FilterNumber=0;	                                           //������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                       //����ģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                       //16λ�� 
#if CAN_RECEIVE_SORT_0                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_0<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_1                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_1<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                     //������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                   //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);			                                         //�˲�����ʼ��
/*****************************************************************************************************************************************************************/	
    CAN_FilterInitStructure.CAN_FilterNumber=1;	                                         //������1
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_2                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_2<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_3                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_3<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������1������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������1
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=2;	                                         //������2
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_4                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_4<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_5                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_5<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������2������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������2
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=3;	                                         //������3
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_6                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_6<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_7                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_7<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������3������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������3
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=4;	                                         //������4
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_8                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_8<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_9                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_9<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������4������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������4
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��  
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=5;	                                         //������5
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_10                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_10<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_11                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_11<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������5������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������5
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=6;	                                         //������6
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_12                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_12<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_13                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_13<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������6������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������6
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=7;	                                         //������7
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //����ģʽ
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16λ��     
#if CAN_RECEIVE_SORT_14                                                                   //������յ� 0  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_14<<5;	                       //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32λMASK   //��������ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //��Դ���ư�ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32λMASK   //��������ID
#endif
#if CAN_RECEIVE_SORT_15                                                                   //������յ� 1  ��ڵ�����
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_15<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //������7������FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //���������7
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //�˲�����ʼ��
/******************************end********************************************************************************************************************************/
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				                        //FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;             // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                    // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
/**********************************************************************
��������: void USB_LP_CAN1_RX0_IRQHandler(void)
��������: �жϷ�����
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 *pFrame;
  CanRxMsg RxMessage;
  CAN_Receive(CAN1, 0, &RxMessage);
	pFrame = Get_Can1_Recv_Pointer(Can1_Receive_Data_Buf);       //���Ҵ洢����λ��
	memcpy((void*)(pFrame + 1), (const void*)(&RxMessage), 20);  //�洢���ݵ����ջ�����
	Can1_Receive_Data_Sign = 1;                                  //������µĿ���ָ��
}
#endif
/**********************************************************************
��������: char* Get_Can1_Recv_Pointer(char* p)
��������: ��ȡ������λ�ô洢����
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
u8* Get_Can1_Recv_Pointer(u8* p)
{
	int i;

	for(i=0;i<Can1_data_number;i++)
	{
		if(0 == *(p+i*(Can1_Data_Lengh +1)))
		{
			*(p+i*(Can1_Data_Lengh +1)) = 1;   //���������Ч
			return (p+i*(Can1_Data_Lengh +1));
		}
	}
	return 0;
}
/**********************************************************************
��������:char* Get_Usart6_Recv_Pointer(char* p)
��������:��ѯ����ָ��
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
u8* Get_Can1_Recv_Data_Pointer(int i ,u8* p)
{
	return (p+i*(Can1_Data_Lengh +1)+1);
}
/**********************************************************************
��������:int Set_Can1_Data_Flag(int i,char* p)
��������:��ѯ����
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
int Set_Can1_Data_Flag(int i,u8* p)
{
	if(1 == *(p+i*(Can1_Data_Lengh +1)))
	{
		*(p+i*(Can1_Data_Lengh +1)) = 0;//�������ȡ��
		return 1;
	}
	else
	{
		return 0;
	}
}
/**********************************************************************
��������: u8* Get_Can1_Reply_Pointer(u8* p)
��������: ��ȡ������λ�ô洢����
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
u8* Get_Can1_Reply_Pointer(u8* p)
{
	int i;

	for(i=0;i<Can1_Reply_data_number;i++)
	{
		if(0 == *(p+i*(Can1_Reply_Data_Lengh +1)))
		{
			*(p+i*(Can1_Reply_Data_Lengh +1)) = 1;   //���������Ч
			return (p+i*(Can1_Reply_Data_Lengh +1));
		}
	}
	i = 0;
	return (p+i*(Can1_Reply_Data_Lengh +1));
}
/**********************************************************************
��������:u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p)
��������:��ѯ����ָ��
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p)
{
	return (p+i*(Can1_Reply_Data_Lengh +1)+1);
}
/**********************************************************************
��������:int Set_Can1_Reply_Data_Flag(int i,u8* p)
��������:��ѯ����
�������:
�� �� ֵ: ����ָ��
***********************************************************************/
int Set_Can1_Reply_Data_Flag(int i,u8* p)
{
	if(1 == *(p+i*(Can1_Reply_Data_Lengh +1)))
	{
//		*(p+i*(Can1_Reply_Data_Lengh +1)) = 0;//�������ȡ��
		return 1;
	}
	else
	{
		return 0;
	}
}



/**********************************************************************
��������: u8 Can_Send_Msg(u8* msg,u8 len ,u16 id)
��������: �������ݺ���
�������:
//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
�� �� ֵ: ����ָ��
***********************************************************************/
u8 Can_Send_Msg(u8* msg,u8 len ,u16 id)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;			                // ��׼��ʶ�� 
	TxMessage.ExtId=0x12;			              // ������չ��ʾ�� 
	TxMessage.IDE=CAN_Id_Standard; 	        // ��׼֡
	TxMessage.RTR=CAN_RTR_Data;		          // ����֡
	TxMessage.DLC=len;				              // Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;	 
}

