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
void analogWrite(BYTE pin, WORD value);
//**********************************************
void setup(void);
void loop(void);
void InitArduinoEnvironment(void);

BYTE zme_strlen(char * str);


// System Datatypes
// -----------------------------------------------------------------
typedef struct _ZUNO_CHANNEL_PROPERTIES_DESCRIPTION
{
	BYTE channel_cmd_class;
	BYTE channel_sensor_type;
	BYTE channel_multilevel_properties;
	GENERIC_POINTER getter;
	GENERIC_POINTER setter;
} ZUNO_CHANNEL_PROPERTIES_DESCRIPTION;

typedef struct _ZUNO_ISR_DESCRIPTION
{
	BYTE insr_num;
	GENERIC_POINTER handler;
} ZUNO_ISR_DESCRIPTION;

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
// -----------------------------------------------------------------

#define ZUNO_SETUP_CHANNELS(...) 	\
								__code ZUNO_CHANNEL_PROPERTIES_DESCRIPTION zunoChannelSetupArray[]= \
								{ \
									{0x42, 0x42, 0x42, 0x4242, 0x4242}, \
									__VA_ARGS__, \
									{0x43, 0x43, 0x43, 0x4343, 0x4343} \
								}
// !! remove
// Reverse compatibility
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  								
#define ZUNO_SETUP_FREQUENCY(VALUE)  BYTE ___dummy_freq
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  								

#define ZUNO_SETUP_DEBUG_MODE(VALUE) 		\
								__code BYTE zunoDebugParameter = VALUE
// System externs
// -----------------------------------------------------------------
extern __code ZUNO_CHANNEL_PROPERTIES_DESCRIPTION zunoChannelSetupArray[];
extern __code ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION zunoAssociationSetupArray[];
extern __code ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION zunoSleepingModeSetupStruct;
extern __code BYTE zunoDebugParameter;
// -----------------------------------------------------------------


/************************************************
			end of Variables
************************************************/


#endif // CORE_ARDUINO_ZUNO__


