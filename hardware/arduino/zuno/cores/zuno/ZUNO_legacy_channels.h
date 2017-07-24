#ifndef ZUNO_LEGACY_CHANNELS
#define ZUNO_LEGACY_CHANNELS
#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

#define VOID_FUNC_POINTER_VOID (void *)
//typedef void (*VOID_FUNC_POINTER_VOID) (void);
typedef void * GENERIC_POINTER;

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
							    ZUNO_CHANNEL_PROPERTIES_DESCRIPTION zunoChannelSetupArray[]= \
								{ \
									__VA_ARGS__, \
								}
#define ZUNO_SETUP_ASSOCIATIONS(...)	\
								 ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION zunoAssociationSetupArray[]= \
								{ \
									__VA_ARGS__, \
								}
#define ZUNO_SETUP_SLEEPING_MODE(VALUE) 		\
								ZUNO_SLEEPING_MODE_PROPERTIES_DESCRIPTION zunoSleepingModeSetupStruct = VALUE

// !! remove
// Reverse compatibility
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  								
#define ZUNO_SETUP_FREQUENCY(VALUE)  BYTE ___dummy_freq
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  								

#define ZUNO_SETUP_DEBUG_MODE(VALUE) 		\
								BYTE zunoDebugParameter = VALUE

#endif // ZUNO_LEGACY_CHANNELS
// -----------------------