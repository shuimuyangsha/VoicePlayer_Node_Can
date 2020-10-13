#include "usart2.h"
#include "sys.h"
#include "time.h"





void Init_Usart2(u32 bound)
	{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
//	USART_InitStructure.USART_Mode = USART_Mode_Tx;	//收发模式
  USART_Init(USART2, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口1 

}
u8 usart2_rend_data_length = 0;       //串口接收数据长度
u8 suart2_send_data_buf[10];          //串口接受数据存储
u16 second_ultrasonic_data = 0;       //接收到的数据  
u32 usart2_send_data_time = 0;        //接收数据的时间
//u8 usart2_new_data_sign = 0;          //窗口接收到新数据标志
void USART2_IRQHandler(void) 
{
	u8 res;
	u8 sum;
	u8 i;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART2);	//读取接收到的数据
		if(usart2_rend_data_length > 4)         //判断数据长度
		{
			usart2_rend_data_length = 0;
			goto end;
		}
		if((0 == usart2_rend_data_length ) && (res != 0xFF))   //判断帧头
		{
			usart2_rend_data_length = 0;
			
			goto end;
		}
		if(3 == usart2_rend_data_length)//判断校验和
		{
			sum = 0;
			for(i=1;i<3;i++)
			{
				sum += suart2_send_data_buf[i];
			}
			if(res != sum)
			{
				usart2_rend_data_length = 0;
				if(0xFF == res)
				{
					suart2_send_data_buf[usart2_rend_data_length] = res;
					usart2_rend_data_length++;
				}
				goto end;
			}
		}
		if(usart2_rend_data_length < 4)   //接收数据
		{
			suart2_send_data_buf[usart2_rend_data_length] = res;
			usart2_rend_data_length++;
      if(4 ==usart2_rend_data_length)         //接受完成	
			{
				second_ultrasonic_data = ((suart2_send_data_buf[1] << 8)& 0xFF00) | suart2_send_data_buf[2];
				usart2_rend_data_length = 0;
				usart2_send_data_time = cnt_time;    //记录数据更新时间
			}				
		}
		end:;	
	}
}
//u8 usart_send_data_buf[10];
//extern u16 first_ultrasonic_data;
//extern u16 second_ultrasonic_data;
//extern u8 error_sign[2];
//void usart2_send_data_buf(u16 Data1,u16 Data2)
//{
//	u16 data1 = 0;
//	u16 data2 = 0;
//	u8 sum = 0;
//	u8 i = 0;
//	data1 = Data1;
//	data2 = Data2;
//	usart_send_data_buf[0] = 0x55;
//	usart_send_data_buf[1] = 0xFF;
//	usart_send_data_buf[2] = 0x01;
//	usart_send_data_buf[2] = usart_send_data_buf[2] | ((error_sign[0] <<7) & 0x80) | ((error_sign[1] <<6) & 0x40);
//	usart_send_data_buf[3] = data1 & 0x00FF;
//	usart_send_data_buf[4] = (data1 >> 8) & 0x00FF;
//	usart_send_data_buf[5] = data2 & 0x00FF;
//	usart_send_data_buf[6] = (data2 >> 8) & 0x00FF;
//	for(i=2;i<7;i++)
//	{
//		sum += usart_send_data_buf[i];
//	}
//	usart_send_data_buf[7] = sum;
//	usart_send_data_buf[8] = 0xFF;
//	usart_send_data_buf[9] = 0xFE;
//}

void UART2_SendCode(u8 *DATA,u8 len)
{
	USART_ClearFlag(USART2,USART_FLAG_TC);    //发送之前清空发送标志  没有这一句 很容易丢包 第一个数据容易丢失
	while(len--)
	{
		USART_SendData(USART2, *DATA++);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
















