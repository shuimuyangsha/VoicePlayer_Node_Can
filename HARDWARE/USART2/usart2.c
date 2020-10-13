#include "usart2.h"
#include "sys.h"
#include "time.h"





void Init_Usart2(u32 bound)
	{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
//	USART_InitStructure.USART_Mode = USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 

}
u8 usart2_rend_data_length = 0;       //���ڽ������ݳ���
u8 suart2_send_data_buf[10];          //���ڽ������ݴ洢
u16 second_ultrasonic_data = 0;       //���յ�������  
u32 usart2_send_data_time = 0;        //�������ݵ�ʱ��
//u8 usart2_new_data_sign = 0;          //���ڽ��յ������ݱ�־
void USART2_IRQHandler(void) 
{
	u8 res;
	u8 sum;
	u8 i;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART2);	//��ȡ���յ�������
		if(usart2_rend_data_length > 4)         //�ж����ݳ���
		{
			usart2_rend_data_length = 0;
			goto end;
		}
		if((0 == usart2_rend_data_length ) && (res != 0xFF))   //�ж�֡ͷ
		{
			usart2_rend_data_length = 0;
			
			goto end;
		}
		if(3 == usart2_rend_data_length)//�ж�У���
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
		if(usart2_rend_data_length < 4)   //��������
		{
			suart2_send_data_buf[usart2_rend_data_length] = res;
			usart2_rend_data_length++;
      if(4 ==usart2_rend_data_length)         //�������	
			{
				second_ultrasonic_data = ((suart2_send_data_buf[1] << 8)& 0xFF00) | suart2_send_data_buf[2];
				usart2_rend_data_length = 0;
				usart2_send_data_time = cnt_time;    //��¼���ݸ���ʱ��
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
	USART_ClearFlag(USART2,USART_FLAG_TC);    //����֮ǰ��շ��ͱ�־  û����һ�� �����׶��� ��һ���������׶�ʧ
	while(len--)
	{
		USART_SendData(USART2, *DATA++);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
















