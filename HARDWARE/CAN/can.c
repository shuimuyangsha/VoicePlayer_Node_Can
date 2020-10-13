#include "can.h"
//#include "gpio.h"
////#include "includes.h"
////#include <stdlib.h>
#include <string.h>

u8 Can1_Receive_Data_Buf[(Can1_Data_Lengh +1) * Can1_data_number] = {0};    //21*20   接收数据缓存区
u8 Can1_Receive_Data_Sign = 0;                                              //标记有新的控制指令
u8* Get_Can1_Recv_Pointer(u8* p);

u8 Can1_Reply_Receive_data_buf[(Can1_Reply_Data_Lengh + 1)*Can1_Reply_data_number] = {0};  /*存储需要回复数据的数组*/


/*****************************************************************************************************************
CAN初始化
tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
则波特率为:36M/((8+9+1)*4)=500Kbps
返回值:0,初始化OK;
    其他,初始化失败; 
*****************************************************************************************************************/
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		    GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE                                       					 									 //是否使能中断
	NVIC_InitTypeDef  		  NVIC_InitStructure;
#endif
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);     					   									 //使能CAN1时钟
#if HARDWARE_VERSIONS_V1_0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     					 									 //使能GOIOB时钟	

  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);            					  								 //复用映射can1
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	          					  								 //复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			           					      								 //初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	          					    								 //上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			           					     									 //初始化IO
#endif
	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			             					       									 //非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;																										 //软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;																										 //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=DISABLE;																										 //禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 																								 //报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=ENABLE;																		 								 //优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	        																						 //模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;																												 //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 																											 //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;																				                 //Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        													                 //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        														                 //初始化CAN1 
/*****************************************************************************************************************************************************************/
	CAN_FilterInitStructure.CAN_FilterNumber=0;	                                           //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                       //掩码模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                       //16位宽 
#if CAN_RECEIVE_SORT_0                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_0<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_1                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_1<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                     //过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                   //激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);			                                         //滤波器初始化
/*****************************************************************************************************************************************************************/	
    CAN_FilterInitStructure.CAN_FilterNumber=1;	                                         //过滤器1
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_2                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_2<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_3                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_3<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器1关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器1
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=2;	                                         //过滤器2
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_4                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_4<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_5                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_5<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器2关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器2
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=3;	                                         //过滤器3
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_6                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_6<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_7                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_7<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器3关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器3
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=4;	                                         //过滤器4
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_8                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_8<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_9                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_9<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器4关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器4
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化  
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=5;	                                         //过滤器5
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_10                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_10<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_11                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_11<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器5关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器5
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=6;	                                         //过滤器6
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_12                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_12<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_13                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_13<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器6关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器6
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/*****************************************************************************************************************************************************************/
    CAN_FilterInitStructure.CAN_FilterNumber=7;	                                         //过滤器7
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	                     //掩码模式
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	                     //16位宽     
#if CAN_RECEIVE_SORT_14                                                                   //定义接收第 0  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_ID_14<<5;	                       //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=CAN_RECEIVE_ID_MASK<<5;                 //32位MASK   //公共调试ID
#else
		CAN_FilterInitStructure.CAN_FilterIdHigh=CAN_RECEIVE_COMMONE_ID<<5;	                 //电源控制板ID_1 0000 0000 0100 0001
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x07FF<<5;                              //32位MASK   //公共调试ID
#endif
#if CAN_RECEIVE_SORT_15                                                                   //定义接收第 1  类节点数据
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_ID_15<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=CAN_RECEIVE_ID_MASK<<5;
#else
		CAN_FilterInitStructure.CAN_FilterIdLow=CAN_RECEIVE_COMMONE_ID<<5;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x07FF<<5;
#endif
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                   //过滤器7关联到FIFO0
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                                 //激活过滤器7
		CAN_FilterInit(&CAN_FilterInitStructure);                                            //滤波器初始化
/******************************end********************************************************************************************************************************/
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				                        //FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;             // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                    // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
/**********************************************************************
函数名称: void USB_LP_CAN1_RX0_IRQHandler(void)
函数功能: 中断服务函数
输入参数:
返 回 值: 数据指针
***********************************************************************/
#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 *pFrame;
  CanRxMsg RxMessage;
  CAN_Receive(CAN1, 0, &RxMessage);
	pFrame = Get_Can1_Recv_Pointer(Can1_Receive_Data_Buf);       //查找存储数据位置
	memcpy((void*)(pFrame + 1), (const void*)(&RxMessage), 20);  //存储数据到接收缓存区
	Can1_Receive_Data_Sign = 1;                                  //标记有新的控制指令
}
#endif
/**********************************************************************
函数名称: char* Get_Can1_Recv_Pointer(char* p)
函数功能: 获取数据真位置存储数据
输入参数:
返 回 值: 数据指针
***********************************************************************/
u8* Get_Can1_Recv_Pointer(u8* p)
{
	int i;

	for(i=0;i<Can1_data_number;i++)
	{
		if(0 == *(p+i*(Can1_Data_Lengh +1)))
		{
			*(p+i*(Can1_Data_Lengh +1)) = 1;   //标记数据有效
			return (p+i*(Can1_Data_Lengh +1));
		}
	}
	return 0;
}
/**********************************************************************
函数名称:char* Get_Usart6_Recv_Pointer(char* p)
函数功能:查询数据指针
输入参数:
返 回 值: 数据指针
***********************************************************************/
u8* Get_Can1_Recv_Data_Pointer(int i ,u8* p)
{
	return (p+i*(Can1_Data_Lengh +1)+1);
}
/**********************************************************************
函数名称:int Set_Can1_Data_Flag(int i,char* p)
函数功能:查询数据
输入参数:
返 回 值: 数据指针
***********************************************************************/
int Set_Can1_Data_Flag(int i,u8* p)
{
	if(1 == *(p+i*(Can1_Data_Lengh +1)))
	{
		*(p+i*(Can1_Data_Lengh +1)) = 0;//标记数据取走
		return 1;
	}
	else
	{
		return 0;
	}
}
/**********************************************************************
函数名称: u8* Get_Can1_Reply_Pointer(u8* p)
函数功能: 获取数据真位置存储数据
输入参数:
返 回 值: 数据指针
***********************************************************************/
u8* Get_Can1_Reply_Pointer(u8* p)
{
	int i;

	for(i=0;i<Can1_Reply_data_number;i++)
	{
		if(0 == *(p+i*(Can1_Reply_Data_Lengh +1)))
		{
			*(p+i*(Can1_Reply_Data_Lengh +1)) = 1;   //标记数据有效
			return (p+i*(Can1_Reply_Data_Lengh +1));
		}
	}
	i = 0;
	return (p+i*(Can1_Reply_Data_Lengh +1));
}
/**********************************************************************
函数名称:u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p)
函数功能:查询数据指针
输入参数:
返 回 值: 数据指针
***********************************************************************/
u8* Get_Can1_Reply_Data_Pointer(int i ,u8* p)
{
	return (p+i*(Can1_Reply_Data_Lengh +1)+1);
}
/**********************************************************************
函数名称:int Set_Can1_Reply_Data_Flag(int i,u8* p)
函数功能:查询数据
输入参数:
返 回 值: 数据指针
***********************************************************************/
int Set_Can1_Reply_Data_Flag(int i,u8* p)
{
	if(1 == *(p+i*(Can1_Reply_Data_Lengh +1)))
	{
//		*(p+i*(Can1_Reply_Data_Lengh +1)) = 0;//标记数据取走
		return 1;
	}
	else
	{
		return 0;
	}
}



/**********************************************************************
函数名称: u8 Can_Send_Msg(u8* msg,u8 len ,u16 id)
函数功能: 发送数据函数
输入参数:
//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
返 回 值: 数据指针
***********************************************************************/
u8 Can_Send_Msg(u8* msg,u8 len ,u16 id)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;			                // 标准标识符 
	TxMessage.ExtId=0x12;			              // 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	        // 标准帧
	TxMessage.RTR=CAN_RTR_Data;		          // 数据帧
	TxMessage.DLC=len;				              // 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}

