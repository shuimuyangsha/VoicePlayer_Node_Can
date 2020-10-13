//#include "gpio.h"

///**************************************************************************
//name    :
//function:
//return  :
//**************************************************************************/
//void Gpio_Init(void)
//{
// GPIO_InitTypeDef  GPIO_InitStructure;
//#if HARDWARE_VERSIONS_1_0
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //使能PA端口时钟
//	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;		//端口配置
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		    //推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    //IO口速度为50MHz
// GPIO_Init(GPIOA, &GPIO_InitStructure);					          //根据设定参数初始化
// 
// GPIO_ResetBits(GPIOA,GPIO_Pin_3);						            //PB.5 输出低
// GPIO_ResetBits(GPIOA,GPIO_Pin_6);						            //PB.5 输出低
//#endif
//}	

///**************************************************************************
//name    :void Close_Power(void)
//function:close facility power
//return  :none
//**************************************************************************/
//void Close_Power(void)
//{
// GPIO_ResetBits(GPIOA,GPIO_Pin_3);						            //PB.5 输出低
// GPIO_ResetBits(GPIOA,GPIO_Pin_6);						            //PB.5 输出低	
//}
///**************************************************************************
//name    :void Open_Power(void)
//function:open facility power
//return  :none
//**************************************************************************/
//void Open_Power(void)
//{
// GPIO_SetBits(GPIOA,GPIO_Pin_3);						             //PB.5 输出高
// GPIO_SetBits(GPIOA,GPIO_Pin_6);						             //PB.5 输出高	
//}

///**************************************************************************
//name    :void Get_Power_State(void)
//function:get work state
//return  :0 no work  1 working
//**************************************************************************/
//u8 Get_Power_State(void)
//{
//	u8 pa3 = 0;
//	u8 pa6 = 0;
//	
//	pa3 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);
//	pa6 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
//	if((1 == pa3) && (1 == pa6))
//	{
//		return 1;	
//	}
//  else
//	{
//	  return 0;
//	}
//}


