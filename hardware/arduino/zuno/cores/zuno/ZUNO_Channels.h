#ifndef ZUNO_CHANNELS
#define ZUNO_CHANNELS

#include "ArduinoTypes.h"


// Channels
typedef struct zuno_channel_s
{
	byte main_type; // Main channel type
	byte sub_type; 	// Sensor type
	byte prop; 		// Properties
	byte channel;
}zuno_channel_t;
typedef struct zuno_sketch_s
{
	byte flags;  // BITS:
				 // |  7  |    6   |   5   |  4  |  3  |  2  |  1  |  0  |
	             // +-----+--------+-------+-----------------+-----+-----+
				 ///| DBG | NOTIF. |SEN_BIN|     S2KEYS		 |FLIRS|SLEEP|
	byte flags2; // BITS:
				 // |  7  |    6   |   5   |  4  |  3  |  2  |  1  |  0  |
	             // +-----+--------+-------+-----+-----+-----+-----+-----+
				 ///|								   |NVMLK|EXT_CHIP_CN|

	byte n_channels;
	byte n_assocs; 
	zuno_channel_t channels[ZUNO_MAX_MULTI_CHANNEL_NUMBER];
	byte asociations[ZUNO_MAX_ASSOC_NUMBER+1];
	// ProductType/Product ID
	BYTE product_type;
	BYTE custom_id_maj;
	BYTE custom_id_min;
	word crc16;
}_xd_zuno_sketch_t;
typedef union ZUNOChannelParam_u
{
	byte 	bParam;
	word	wParam;
	dword 	dwParam;
	byte    buffParam[4];
}ZUNOChannelParam_t;
typedef struct ZUNOChannelHandler_s
{
	BYTE type;
	BYTE channel; // #EP/#cfg_param
	ZUNOChannelParam_t param1;
	ZUNOChannelParam_t param2;
	ZUNOChannelParam_t result;  // Length of outgoing packet
}_xd_ZUNOChannelHandler_t;
/*
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
*/
typedef struct ZUNOSysCallData_s
{
	byte 				syscall_num;
	ZUNOChannelParam_t	ret_value;
	byte				param_data[ZUNO_MAX_PARAMDATA];

}_xd_ZUNOSysCallData_t;
typedef struct ZUNOThermostatData_s{
	byte mode;
	word temperature[2];
}ZUNOThermostatData_t;
typedef union ZUNOGeneralChannelData_u
{
	byte 					bParam;
	word					wParam;
	dword 					dwParam;
	byte    				buffParam[8];
	ZUNOThermostatData_t    thermoParam;
}ZUNOGeneralChannelData_t;

#ifndef __ZUNO_RAW_C
extern _xd_ZUNOChannelHandler_t callback_data;
extern _xd_ZUNOSysCallData_t syscall_data;
extern _xd_zuno_sketch_t g_user_sketch;
extern _xd_zuno_sketch_t g_accepted_sketch;
extern XBYTE g_ptr_config[MAX_GLOBAL_CFG];

#define SYSRET_B	syscall_data.ret_value.bParam	
#define SYSRET_W    syscall_data.ret_value.wParam
#define SYSRET_DW	syscall_data.ret_value.dwParam
#define SYSRET_BUFF(X)	syscall_data.ret_value.buffParam[X]

void zunoSysCall(byte syscall_vector, ...);
void zunoCall();
#endif // __ZUNO_RAW_C


#endif // ZUNO_CHANNELS