#ifndef ZUNO_CHANNELS
#define ZUNO_CHANNELS

#include "ArduinoTypes.h"


// Channels
typedef struct zuno_channel_s
{
	byte main_type; // Main channel type
	byte sub_type; 	// Sensor type
	byte prop; 		// Properties
}zuno_channel_t;
typedef struct zuno_sketch_s
{
	byte flags;  // BITS:
				 // |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
	             // +-----+-----------------------------+-----+-----+
				 ///| DBG |  		 RESERVED  			|FLIRS|SLEEP|
			
	byte n_channels;
	byte n_assocs; 
	zuno_channel_t channels[ZUNO_MAX_MULTI_CHANNEL_NUMBER];
	byte asociations[ZUNO_MAX_ASSOC_NUMBER];
	word crc16;
}_xd_zuno_sketch_t;
typedef union ZUNOChannelParam_u
{
	byte 	bParam;
	word	wParam;
	dword 	dwParam;
	byte    buffParam[4];
}ZUNOChannelParam_t;
/*
typedef struct ZunoCallbackData_s
{
	BYTE callback_vector; // number << 1 + 0/1 (1 == setter, 0 == getter)
	ZunoParamData_t param;
	// Используется для репортов
	// дополнительные данные
	// -----------------------------------------------
	BYTE type; // CC_BASIS/CC_SENSOR_MULTILEVEL/CC_SENSOR_BINARY/CC_METER
	BYTE sub_type;
	BYTE scale;
	BYTE groupId; // Для идентификации репорта внутри кода Z-Uno
	BYTE nodeId; // Для отладки/экспериментов
	// -----------------------------------------------
}ZunoCallbackData_t;
*/
typedef struct ZUNOChannelHandler_s
{
	byte 				type;
	ZUNOChannelParam_t	param;
	// Extra data, used for reports
	// -----------------------------------------------
	BYTE main_type; // CC_BASIS/CC_SENSOR_MULTILEVEL/CC_SENSOR_BINARY/CC_METER
	BYTE sub_type;
	BYTE scale;
	BYTE groupId;// To identify the report 
	BYTE nodeId; // Z-Wave NodeID (for Debug/Experimental purposes)
	// -----------------------------------------------
}_xd_ZUNOChannelHandler_t;

typedef struct ZUNOSysCallData_s
{
	byte 				syscall_num;
	ZUNOChannelParam_t	ret_value;
	byte				param_data[ZUNO_MAX_PARAMDATA];

}_xd_ZUNOSysCallData_t;

#ifndef __ZUNO_RAW_C
extern _xd_ZUNOChannelHandler_t callback_data;
extern _xd_ZUNOSysCallData_t syscall_data;
extern _xd_zuno_sketch_t  g_user_sketch;
extern XBYTE g_ptr_config[MAX_GLOBAL_CFG];

#define SYSRET_B	syscall_data.ret_value.bParam	
#define SYSRET_W    syscall_data.ret_value.wParam
#define SYSRET_DW	syscall_data.ret_value.dwParam
#define SYSRET_BUFF(X)	syscall_data.ret_value.buffParam[X]

void zunoSysCall(byte syscall_vector, ...);
void zunoCall();
#endif // __ZUNO_RAW_C


#endif // ZUNO_CHANNELS