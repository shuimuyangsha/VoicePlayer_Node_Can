#ifndef __SYS_CONFIG_H
#define __SYS_CONFIG_H	 




#define ULTRASONUND_1_NUMBER                    0x40       //���� 1 �� �忨
#define ULTRASONUND_2_NUMBER                    0x41       //���� 2 �� �忨
#define ULTRASONUND_3_NUMBER                    0x42       //���� 3 �� �忨
#define ULTRASONUND_4_NUMBER                    0x43       //���� 4 �� �忨
#define ULTRASONUND_5_NUMBER                    0x44       //���� 5 �� �忨  
#define ULTRASONUND_6_NUMBER                    0x45       //���� 6 �� �忨
#define ULTRASONUND_7_NUMBER                    0x46       //���� 7 �� �忨
#define ULTRASONUND_8_NUMBER                    0x47       //���� 8 �� �忨  

#define ULTRASONUND_9_NUMBER                    0x48       //���� 9  �� �忨
#define ULTRASONUND_10_NUMBER                   0x49       //���� 10 �� �忨
#define ULTRASONUND_11_NUMBER                   0x4A       //���� 11 �� �忨
#define ULTRASONUND_12_NUMBER                   0x4B       //���� 12 �� �忨
#define ULTRASONUND_13_NUMBER                   0x4C       //���� 13 �� �忨  
#define ULTRASONUND_14_NUMBER                   0x4D       //���� 14 �� �忨
#define ULTRASONUND_15_NUMBER                   0x4E       //���� 15 �� �忨
#define ULTRASONUND_16_NUMBER                   0x4F       //���� 16 �� �忨  


#define CAN_HARDWARE_NUMBER  ULTRASONUND_1_NUMBER      //�豸�ڵ�1




/*
ID����
����λ   ����б��
����λ   ���ݽ������
����λ   ���ݷ������
*/
//����������
#define CAN_RECEIVE_SORT_0               0       //������յ� 0  ��ڵ�����
#define CAN_RECEIVE_SORT_1               0       //������յ� 1  ��ڵ�����
#define CAN_RECEIVE_SORT_2               0       //������յ� 2  ��ڵ�����
#define CAN_RECEIVE_SORT_3               1       //������յ� 3  ��ڵ�����
#define CAN_RECEIVE_SORT_4               0       //������յ� 4  ��ڵ�����
#define CAN_RECEIVE_SORT_5               0       //������յ� 5  ��ڵ�����
#define CAN_RECEIVE_SORT_6               0       //������յ� 6  ��ڵ�����
#define CAN_RECEIVE_SORT_7               0       //������յ� 7  ��ڵ�����
#define CAN_RECEIVE_SORT_8               0       //������յ� 8  ��ڵ�����
#define CAN_RECEIVE_SORT_9               0       //������յ� 9  ��ڵ�����
#define CAN_RECEIVE_SORT_10              0       //������յ� 10 ��ڵ�����
#define CAN_RECEIVE_SORT_11              0       //������յ� 11 ��ڵ�����
#define CAN_RECEIVE_SORT_12              0       //������յ� 12 ��ڵ�����
#define CAN_RECEIVE_SORT_13              0       //������յ� 13 ��ڵ�����
#define CAN_RECEIVE_SORT_14              0       //������յ� 14 ��ڵ�����
#define CAN_RECEIVE_SORT_15              0       //������յ� 15 ��ڵ�����
//#define CAN_RECEIVE_SORT_COMMONE         0       //������յ� ���� ��ڵ�����


#define CAN_RECEIVE_ID_0                 0x0000  //������� ID 
#define CAN_RECEIVE_ID_1                 0x0001  //������� ID 
#define CAN_RECEIVE_ID_2                 0x0002  //������� ID 
#define CAN_RECEIVE_ID_3                 0x0003  //������� ID 
#define CAN_RECEIVE_ID_4                 0x0004  //������� ID 
#define CAN_RECEIVE_ID_5                 0x0005  //������� ID 
#define CAN_RECEIVE_ID_6                 0x0006  //������� ID 
#define CAN_RECEIVE_ID_7                 0x0007  //������� ID 
#define CAN_RECEIVE_ID_8                 0x0008  //������� ID 
#define CAN_RECEIVE_ID_9                 0x0009  //������� ID 
#define CAN_RECEIVE_ID_10                0x000A  //������� ID 
#define CAN_RECEIVE_ID_11                0x000B  //������� ID 
#define CAN_RECEIVE_ID_12                0x000C  //������� ID 
#define CAN_RECEIVE_ID_13                0x000D  //������� ID 
#define CAN_RECEIVE_ID_14                0x000E  //������� ID 
#define CAN_RECEIVE_ID_15                0x000F  //������� ID 
#define CAN_RECEIVE_COMMONE_ID           0x07FE  //������� ID (����) 
#define CAN_RECEIVE_ID_MASK              0x000F  //������� ID 








//����Ӳ���汾��Ϣ  ����Ӳ����ʼ��
#define HARDWARE_VERSIONS_V1_0       1
//#define HARDWARE_VERSIONS_V2_0       0


#endif
