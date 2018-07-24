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
	byte asociations[ZUNO_MAX_ASSOC_NUMBER+1]; 
	word crc16;
}zuno_sketch_t;
typedef union ZUNOChannelParam_u
{
	byte 	bParam;
	word	wParam;
	DWORD 	dwParam;
	byte    buffParam[4];
}ZUNOChannelParam_t;

typedef struct ZUNOChannelHandler_s
{
	byte 				type;
	ZUNOChannelParam_t	param;
	// Extended data for thermostat/color channels & reports  
	// -----------------------------------------------
	BYTE command_class; // CC_BASIS/CC_SENSOR_MULTILEVEL/CC_SENSOR_BINARY/CC_METER
	BYTE sub_type;
	BYTE scale;
	BYTE groupId; // Group where NodeId was added
	BYTE nodeId; // for debugging
	// -----------------------------------------------
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