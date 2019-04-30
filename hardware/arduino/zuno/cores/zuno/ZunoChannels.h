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
				 // |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
	             // +-----+-----------------------------+-----+-----+
				 ///| DBG |  		 RESERVED  			|FLIRS|SLEEP|
			
	byte n_channels;
	byte n_assocs; 
	zuno_channel_t channels[ZUNO_MAX_MULTI_CHANNEL_NUMBER];
	byte asociations[ZUNO_MAX_ASSOC_NUMBER+1]; 
	// ProductType/Product ID
	BYTE product_type;
	BYTE custom_id_maj;
	BYTE custom_id_min;
	word crc16;
}zuno_sketch_t;
typedef union ZUNOChannelParam_u
{
	byte 	bParam;
	word	wParam;
	DWORD 	dwParam;
	byte    buffParam[4];
}ZUNOChannelParam_t;

typedef union ZUNOCallbackParam_u
{
	byte 	bParam;
	word	wParam;
	DWORD 	dwParam;
	byte    buffParam[14];
}ZUNOCallbackParam_t;


typedef struct ZUNOChannelHandler_s
{
	BYTE type;
	BYTE channel; // #EP/#cfg_param
	ZUNOChannelParam_t param1;
	ZUNOChannelParam_t param2;
	ZUNOChannelParam_t result;  // Length of outgoing packet
}ZUNOChannelHandler_t;

typedef union ZUNOReportParam_u
{
	byte 	bParam;
	word	wParam;
	DWORD 	dwParam;
	byte    buffParam[4];
}ZUNOReportParam_t;

typedef struct ZUNOReportValue
{
	ZUNOReportParam_t	param;
	// Extra data, used for reports
	// -----------------------------------------------
	BYTE main_type; // CC_BASIS/CC_SENSOR_MULTILEVEL/CC_SENSOR_BINARY/CC_METER
	BYTE sub_type;
	BYTE scale;
	BYTE groupId;// To identify the report 
	BYTE nodeId; // Z-Wave NodeID (for Debug/Experimental purposes)
	// -----------------------------------------------
}ZUNOReportValue_t;
extern ZUNOChannelHandler_t * callback_data;
extern zuno_sketch_t * g_user_sketch;
extern byte * g_ptr_config;


#endif // ZUNO_CHANNELS