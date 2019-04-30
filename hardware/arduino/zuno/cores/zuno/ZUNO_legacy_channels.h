#ifndef ZUNO_LEGACY_CHANNELS
#define ZUNO_LEGACY_CHANNELS
#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

#define VOID_FUNC_POINTER_VOID (void *)
typedef void * GENERIC_POINTER;

// System Datatypes
// -----------------------------------------------------------------
typedef struct _ZUNO_CHANNEL_PROPERTIES_DESCRIPTION
{
	BYTE channel_cmd_class;
	BYTE channel_sensor_type;
	BYTE channel_multilevel_properties;
	GENERIC_POINTER getter1;
	GENERIC_POINTER setter1;
	GENERIC_POINTER getter2;
	GENERIC_POINTER setter2;
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

#define ZUNO_SETUP_BATTERY_LEVELS(L,H) 		\
								word zunoBatteryLow = L; \
								word zunoBatteryHigh = H 
								
#define ZUNO_SETUP_CFGPARAMETER_HANDLER(H) 		\
								GENERIC_POINTER zunoCFGHandler = ((void*)H)
#define ZUNO_SETUP_BATTERY_HANDLER(H) 		\
								GENERIC_POINTER zunoBatteryHandler = ((void*)H)
#define ZUNO_REPORTS_HANDLER(N, H) 		\
								GENERIC_POINTER zunoReportsHandler_##N = ((void*)H)
#define ZUNO_SETUP_SYSEVENT_HANDLER(H)\
								GENERIC_POINTER zunoEventHandler = ((void*)H)
#define ZUNO_SETUP_SYSTIMER_HANDLER(H)\
								GENERIC_POINTER zunoSysTimerHandler = ((void*)H)

#define ZUNO_SETUP_PRODUCT_AUTO() \
								BYTE zunoProductType = 1
#define ZUNO_SETUP_PRODUCT_ID(MAJ,MIN) \
								BYTE zunoProductType = 2;\
								BYTE zunoProductIDMaj = MAJ;\
								BYTE zunoProductIDMin = MIN

#define ZUNO_REMAP_SPINS(PROFILE) \
								char zunoREMAPProfile[] = #PROFILE
#define ZUNO_SETUP_S2ACCESS(B) \
								BYTE zunoS2Access = B
#define ZUNO_SETUP_FWUPGRADE(COUNT, READY_HANDLER, META_HANDLER) \
								char zunoFWUpgradeCount = COUNT;\
								GENERIC_POINTER zunoFWUpgradeReadyH = ((void*)READY_HANDLER);\
								GENERIC_POINTER zunoFWUpgradeMetaH = ((void*)META_HANDLER);
#define ZUNO_DISABLE(VAR_NAMES)\
								char zunoDisableList[] = #VAR_NAMES
#define ZUNO_ENABLE(VAR_NAMES)\
								char zunoEnableList[] = #VAR_NAMES
#define ZUNO_DYNAMIC_CHANNELS(NUM_CHANNELS)\
								char zunoMaxDynamicChannels = NUM_CHANNELS;
#endif // ZUNO_LEGACY_CHANNELS
// -----------------------