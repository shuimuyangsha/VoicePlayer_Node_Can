#ifndef __SYS_CONFIG_H
#define __SYS_CONFIG_H	 




#define ULTRASONUND_1_NUMBER                    0x40       //超声 1 号 板卡
#define ULTRASONUND_2_NUMBER                    0x41       //超声 2 号 板卡
#define ULTRASONUND_3_NUMBER                    0x42       //超声 3 号 板卡
#define ULTRASONUND_4_NUMBER                    0x43       //超声 4 号 板卡
#define ULTRASONUND_5_NUMBER                    0x44       //超声 5 号 板卡  
#define ULTRASONUND_6_NUMBER                    0x45       //超声 6 号 板卡
#define ULTRASONUND_7_NUMBER                    0x46       //超声 7 号 板卡
#define ULTRASONUND_8_NUMBER                    0x47       //超声 8 号 板卡  

#define ULTRASONUND_9_NUMBER                    0x48       //超声 9  号 板卡
#define ULTRASONUND_10_NUMBER                   0x49       //超声 10 号 板卡
#define ULTRASONUND_11_NUMBER                   0x4A       //超声 11 号 板卡
#define ULTRASONUND_12_NUMBER                   0x4B       //超声 12 号 板卡
#define ULTRASONUND_13_NUMBER                   0x4C       //超声 13 号 板卡  
#define ULTRASONUND_14_NUMBER                   0x4D       //超声 14 号 板卡
#define ULTRASONUND_15_NUMBER                   0x4E       //超声 15 号 板卡
#define ULTRASONUND_16_NUMBER                   0x4F       //超声 16 号 板卡  


#define CAN_HARDWARE_NUMBER  ULTRASONUND_1_NUMBER      //设备节点1




/*
ID定义
高三位   类别中编号
中四位   数据接收类别
低四位   数据发送类别
*/
//定义接收类别
#define CAN_RECEIVE_SORT_0               0       //定义接收第 0  类节点数据
#define CAN_RECEIVE_SORT_1               0       //定义接收第 1  类节点数据
#define CAN_RECEIVE_SORT_2               0       //定义接收第 2  类节点数据
#define CAN_RECEIVE_SORT_3               1       //定义接收第 3  类节点数据
#define CAN_RECEIVE_SORT_4               0       //定义接收第 4  类节点数据
#define CAN_RECEIVE_SORT_5               0       //定义接收第 5  类节点数据
#define CAN_RECEIVE_SORT_6               0       //定义接收第 6  类节点数据
#define CAN_RECEIVE_SORT_7               0       //定义接收第 7  类节点数据
#define CAN_RECEIVE_SORT_8               0       //定义接收第 8  类节点数据
#define CAN_RECEIVE_SORT_9               0       //定义接收第 9  类节点数据
#define CAN_RECEIVE_SORT_10              0       //定义接收第 10 类节点数据
#define CAN_RECEIVE_SORT_11              0       //定义接收第 11 类节点数据
#define CAN_RECEIVE_SORT_12              0       //定义接收第 12 类节点数据
#define CAN_RECEIVE_SORT_13              0       //定义接收第 13 类节点数据
#define CAN_RECEIVE_SORT_14              0       //定义接收第 14 类节点数据
#define CAN_RECEIVE_SORT_15              0       //定义接收第 15 类节点数据
//#define CAN_RECEIVE_SORT_COMMONE         0       //定义接收第 公共 类节点数据


#define CAN_RECEIVE_ID_0                 0x0000  //定义接收 ID 
#define CAN_RECEIVE_ID_1                 0x0001  //定义接收 ID 
#define CAN_RECEIVE_ID_2                 0x0002  //定义接收 ID 
#define CAN_RECEIVE_ID_3                 0x0003  //定义接收 ID 
#define CAN_RECEIVE_ID_4                 0x0004  //定义接收 ID 
#define CAN_RECEIVE_ID_5                 0x0005  //定义接收 ID 
#define CAN_RECEIVE_ID_6                 0x0006  //定义接收 ID 
#define CAN_RECEIVE_ID_7                 0x0007  //定义接收 ID 
#define CAN_RECEIVE_ID_8                 0x0008  //定义接收 ID 
#define CAN_RECEIVE_ID_9                 0x0009  //定义接收 ID 
#define CAN_RECEIVE_ID_10                0x000A  //定义接收 ID 
#define CAN_RECEIVE_ID_11                0x000B  //定义接收 ID 
#define CAN_RECEIVE_ID_12                0x000C  //定义接收 ID 
#define CAN_RECEIVE_ID_13                0x000D  //定义接收 ID 
#define CAN_RECEIVE_ID_14                0x000E  //定义接收 ID 
#define CAN_RECEIVE_ID_15                0x000F  //定义接收 ID 
#define CAN_RECEIVE_COMMONE_ID           0x07FE  //定义接收 ID (公共) 
#define CAN_RECEIVE_ID_MASK              0x000F  //定义接收 ID 








//定义硬件版本信息  决定硬件初始化
#define HARDWARE_VERSIONS_V1_0       1
//#define HARDWARE_VERSIONS_V2_0       0


#endif
