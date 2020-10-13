#ifndef __VERSION_INFORMATION__
#define __VERSION_INFORMATION__
#include "sys.h"

#define MotherBoard_DeviceName	01			//嵌入式主板
#define SoftwareVersion			    11			//11 表示V1.1
#define HardwareVersion			    01			//嵌入式主板


#pragma pack (1)
typedef struct	
{
	u16 RecordDeviceName;					         //设备名称
	u16 HardwareVersionInformation;	       //硬件版本信息
	u16 SoftwareVersionInformation;	       //软件版本信息
	uint64_t RecordFinalCompileTime;			 //编译时间 (Unix标准时间戳, 自1970年1月1日0:0:0起计算的秒数)
} VersionInformation;
#pragma pack ()

extern const VersionInformation g_configParamDefault;


#endif
