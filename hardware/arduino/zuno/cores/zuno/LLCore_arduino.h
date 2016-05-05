#ifndef CORE_ARDUINO_ZUNO__
#define CORE_ARDUINO_ZUNO__

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

typedef __code void (*VOID_FUNC_POINTER_VOID) (void);
typedef __code void (*VOID_FUNC_POINTER_BYTE) (BYTE value);
typedef __code void (*VOID_FUNC_POINTER_DW0RD) (DWORD value);
typedef __code BYTE (*BYTE_FUNC_POINTER_VOID) (void);
typedef __code WORD (*WORD_FUNC_POINTER_VOID) (void);
typedef __code DWORD (*DWORD_FUNC_POINTER_VOID) (void);
typedef __code void (*VOID_FUNC_POINTER_BUFFER) (const char *bufPointer);
typedef __code void (*VOID_FUNC_POINTER_BUFFER_BYTE) (const char *bufPointer, BYTE value);
typedef __code void * GENERIC_POINTER;


typedef struct _ZUNO_CHANNEL_PROPERTIES_DESCRIPTION
{
	BYTE channel_cmd_class;
	BYTE channel_sensor_type;
	BYTE channel_multilevel_properties;
	GENERIC_POINTER getter;
	GENERIC_POINTER setter;
} ZUNO_CHANNEL_PROPERTIES_DESCRIPTION;


typedef struct _ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION
{
	BYTE association_type;
	BYTE association_param;
} ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION;

typedef struct _ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION
{
	BYTE current_mode;
	BYTE parameter;
	GENERIC_POINTER handler;
} ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION;

//#define XBYTE  _xdata BYTE /* External data byte */



#define ZUNO_SETUP_CHANNELS(...) 	\
								__code ZUNO_CHANNEL_PROPERTIES_DESCRIPTION zunoChannelSetupArray[]= \
								{ \
									{0x42, 0x42, 0x42, 0x4242, 0x4242}, \
									__VA_ARGS__, \
									{0x43, 0x43, 0x43, 0x4343, 0x4343} \
								}





enum {
	ZUNO_ASSOC_BASIC_SET_NUMBER = 1, 						//0x01
	ZUNO_ASSOC_BASIC_SET_AND_DIM_NUMBER,				//0x02
	ZUNO_ASSOC_SCENE_ACTIVATION_NUMBER, 				//0x03
	ZUNO_END_OF_SUPPORTED_ASSOC_NUM,
};

#define ZUNO_ASSOC_NO_PARAMS 							0x00

#define ZUNO_NO_ASSOCIATIONS							{0, 0}
#define ZUNO_ASSOCIATION_GROUP_SET_VALUE 				{ZUNO_ASSOC_BASIC_SET_NUMBER, ZUNO_ASSOC_NO_PARAMS}
#define ZUNO_ASSOCIATION_GROUP_SET_VALUE_AND_DIM 		{ZUNO_ASSOC_BASIC_SET_AND_DIM_NUMBER, ZUNO_ASSOC_NO_PARAMS}
#define ZUNO_ASSOCIATION_GROUP_SCENE_CONTROL 			{ZUNO_ASSOC_SCENE_ACTIVATION_NUMBER, ZUNO_ASSOC_NO_PARAMS}
//TODO #define ZUNO_ASSOCIATION_GROUP_COLOR_CONTROL 		 		{}
//TODO #define ZUNO_ASSOCIATION_GROUP_THERMOSTAT_CONTROL 		 	{}
//TODO #define ZUNO_ASSOCIATION_GROUP_DOOR_LOCK_CONTROL 		 	{}

#define ZUNO_SETUP_ASSOCIATIONS(...)	\
								__code ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION zunoAssociationSetupArray[]= \
								{ \
									{0x42, 0x42}, \
									__VA_ARGS__, \
									{0x43, 0x43} \
								}


enum {
	ZUNO_MODE_ALWAYS_LISTENING_NUMBER,				//0
	ZUNO_MODE_WAKE_UP_NUMBER,						//1
	ZUNO_MODE_FLIRS_NUMBER,							//2
};

#define ZUNO_SLEEPING_MODE_ALWAYS_AWAKE 			{ZUNO_MODE_ALWAYS_LISTENING_NUMBER, 0, 0}
#define ZUNO_SLEEPING_MODE_SLEEPING			 		{ZUNO_MODE_WAKE_UP_NUMBER, 0, 0}
#define ZUNO_SLEEPING_MODE_FREQUENTLY_AWAKE 		{ZUNO_MODE_FLIRS_NUMBER, 0, 0}
#define ZUNO_SETUP_SLEEPING_MODE(VALUE) 		\
								__code ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION zunoSleepingModeSetupStruct = VALUE

				
//!! remove
#define ZUNO_SETUP_FREQUENCY(VALUE) 		BYTE dummy

#define ZUNO_SETUP_DEBUG_MODE(VALUE) 		\
								__code BYTE zunoDebugParameter = VALUE


#define ZUNO_MAX_MULTI_CHANNEL_NUMBER 					10





//Debug///
BYTE zunoPopByte(void);
WORD zunoPopWord(void);
DWORD zunoPopDWORD(void);
void zunoPushByte(BYTE value);
void zunoPushWord(WORD value);
void zunoPushDword(DWORD value);
void zunoCall(void);
//********************************
void pinMode(BYTE pin, BYTE mode);
BYTE digitalRead(BYTE pin);
void digitalWrite(BYTE pin, BYTE value);
void delay(DWORD value);
WORD analogRead(BYTE pin);
void analogWrite(BYTE pin, BYTE value);
//**********************************************
void setup(void);
void loop(void);
void InitArduinoEnvironment(void);

BYTE zme_strlen(char * str);


#define ZUNO_REPORT_NO_IMMEDIATE_VALUE 		0xffff
#define zunoSendReport(CHANNEL)  zunoSendUncolicitedReport(CHANNEL,ZUNO_REPORT_NO_IMMEDIATE_VALUE)
void zunoSendUncolicitedReport(BYTE channel,WORD value);

#define ZUNO_DIMMING_UP				0
#define ZUNO_DIMMING_DOWN 			1
#define ZUNO_DIMMING_START			1
#define ZUNO_DIMMING_STOP 			0

#define zunoSendToGroupSetValueCommand(GROUP,VALUE) 					zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_BASIC_SET_NUMBER,VALUE,0)
#define zunoSendToGroupDimmingCommand(GROUP,DIRECTION,START_STOP) 		zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_BASIC_SET_AND_DIM_NUMBER,DIRECTION,START_STOP)
#define zunoSendToGroupScene(GROUP,SCENE_NUMBER) 						zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_SCENE_ACTIVATION_NUMBER,SCENE_NUMBER,0)

void zunoSendAssociationCommand(BYTE group, BYTE assoc_type, BYTE param1, BYTE param2);

void zunoSendDeviceToSleep(void);


/************************************************
			Variables
************************************************/
extern __code ZUNO_CHANNEL_PROPERTIES_DESCRIPTION zunoChannelSetupArray[];
extern __code ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION zunoAssociationSetupArray[];
extern __code ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION zunoSleepingModeSetupStruct;
extern __code BYTE zunoDebugParameter;



/************************************************
			end of Variables
************************************************/


#endif // CORE_ARDUINO_ZUNO__


