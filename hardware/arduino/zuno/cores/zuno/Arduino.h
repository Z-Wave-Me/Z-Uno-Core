#pragma once
#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"
#include "binary.h" // to make somebody that don't use hexadecimal values happy
// Additional libraries with "pluses"
#include "HardwareSerial.h" // Serial support
// All common c-style fubctions
#include "ArduinoCAPI.h"
#include "ZUNO_Channels.h"
#include "HLCore.h"
// Хидеры SDCC
#include "math.h"
#include "stdlib.h"

#include "Custom_pins.h"
#include "Custom_defs.h"

// To reduce code & stack usage
// we use macroses instead of functions
//#define analogRead(P,V)                                                    g_gpio_pin=P;g_gpio_wval=V;rawAnalogWrite()
//#define millis()                                                            (g_ms_counter += zunoGI("TMR")),zunoSI("TMR",0);
#define zunoNID()                                                           (g_nzram_NID)
#define zunoInNetwork()                                                     (g_nzram_NID != 0)        
#define analogWrite(P,V)                                                    g_gpio_pin=P;g_gpio_wval=V;rawAnalogWrite()
#define analogWriteResolution(B) 											g_ptr_config[ZUNO_CFG_BYTE_PWM_RES] = B	
#define analogReference(B)													g_ptr_config[ZUNO_CFG_BYTE_ADC_REF] = B
#define analogReadResolution(B) 											g_ptr_config[ZUNO_CFG_BYTE_ADC_RES] = B	
#define zunoADCAdvConfig(B)													g_ptr_config[ZUNO_CFG_BYTE_ADC_ADV] = B
#define zunoSendDeviceToSleep()												g_ptr_config[ZUNO_CFG_BYTE_SLEEPLOCK] = FALSE
#define zunoGetWakeReason()													g_ptr_config[ZUNO_CFG_BYTE_WUP_REASON] 
// Create a new Tx-packet
#define zunoCreatePacket()                                                  zunoSysCall(ZUNO_FUNC_CREATEPACKET)
// Update device channels/associations
#define zunoCommitConfig() 													zunoSysCall(ZUNO_FUNC_COMMIT_CONFIG)
// Z-Wave communication
#define zunoSendUncolicitedReport(channel) 									rawSendUnsolicitedReport((byte)(long long)(channel)) 

#define zunoSendDirectReport(group, length)                                 zunoSysCall(ZUNO_FUNC_ASSOCIATION_SEND, byte(group), byte(length))                                                                            //zunoSysCall(ZUNO_FUNC_UNSOLICITED_REPORT, byte(channel))
#define zunoSendAssociationCommand(group, assoc_type, param1, param2) 		zunoSysCall(ZUNO_FUNC_ASSOCIATION_SEND, byte(group), byte(assoc_type), byte(param1), byte(param2))
#define	zunoStartLearn(timeout, secure)										zunoSysCall(ZUNO_FUNC_LEARN, byte(timeout), byte(secure))
// OTA cofirmation
#define	zunoConfigFWUpdate(my_version,	unlock_pin) 						zunoSysCall(ZUNO_FUNC_SETUP_FWUPGRADE, word(my_version), dword(unlock_pin))
/*
extern XBYTE g_nzram_BeamCount;
extern XBYTE g_nzram_WUOpts;
*/
#define zunoSetWUOptions(options)											g_nzram_WUOpts = options// zunoSysCall(ZUNO_FUNC_INT0_WUP_LEVEL, byte(options))
#define zunoSetBeamCountWU(count)											g_nzram_BeamCount = count//zunoSysCall(ZUNO_FUNC_BEAMCOUNT, byte(count))
//#define analogWrite(pin, value)												rawAnalogWrite(pin, value)//zunoSysCall(ZUNO_FUNC_ANALOG_WRITE, byte(pin), word(value))
#define analogRead(pin)                                                     rawAnalogRead(pin)
// Z-Wave helpers
#define zunoSendReport(CHANNEL)                                             zunoSendUncolicitedReport(CHANNEL)
#define zunoSaveCFGParam(CHANNEL, VAL)                                      callback_data.param1.bParam = CHANNEL; callback_data.param2.wParam = VAL; rawSaveCFGParam();

#define zunoAcceptFirmware()                                                g_ptr_config[ZUNO_CFG_BYTE_FWAUTH] = TRUE
/*
#define zunoSendDbgData(group, data, data_size)                             zunoSysCall(ZUNO_FUNC_DBGSENDDATA, byte(group), byte(data_size), data)
#define zunoSendToGroupSetValueCommand(GROUP,VALUE) 		zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_BASIC_SET_NUMBER,VALUE,0)
#define zunoSendToGroupDimmingCommand(GROUP,DIRECTION,START_STOP) 	zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_BASIC_SET_AND_DIM_NUMBER,DIRECTION,START_STOP)
#define zunoSendToGroupScene(GROUP,SCENE_NUMBER) 			zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_SCENE_ACTIVATION_NUMBER,SCENE_NUMBER,0)
#define zunoSendToGroupDoorlockControl(GROUP,OPEN_CLOSE) 	zunoSendAssociationCommand(GROUP,ZUNO_ASSOC_DOORLOCK_CONTROL_NUMBER,OPEN_CLOSE,0)	
*/
// Backward compatibility			
// Deprecated functions
#define zunoSetupKeyScannerWU(W) 	zunoSetWUOptions( W > 0 ? ZUNO_WUPFLAGS_INT1_KEYSCAN : 0x00)				
															
//#define delay(value)														zunoPushSysParam(0, dword(value)); Delay()
// SDCC uses only float parameters for math and names functions with "f" suffix.
// Arduino uses double variants of math functions without "f".
// We have to make life easier for all ;-) 
#define sin(x) 			sinf(x)
#define cos(x) 			cosf(x)
#define tan(x) 			tanf(x)
#define cot(x) 			cotf(x)
#define asin(x) 		asinf(x)
#define acos(x) 		acosf(x)
#define atan(x) 		atanf(x)
#define atan2(x, y) 	atan2f(x, y)
#define sinh(x) 		sinhf(x)
#define cosh(x) 		coshf(x)
#define tanh(x) 		tanhf(x)
#define exp(x) 			expf(x)
#define log(x) 			logf(x)
#define log10(x) 		log10f(x)
#define pow(x,y) 		powf(x,y)
#define sqrt(x) 		sqrtf(x)
#define fabs(x)     	fabsf(x)
#define ceil(x)     	ceilf(x)
#define floor(x)    	floorf(x)
#define modf(x,y)   	modff(x,y)
#define frexp(x, pw2)   frexpf(x, pw2)
#define ldexp(x, pw2)   ldexpf(x, pw2)


// Версия Arduino
#define ARDUINO 		    152	
#define DEG_TO_RAD 			0.017453292519
#define RAD_TO_DEG 			57.29577951308
#define EULER 				2.718281828459

#define min(a,b) 				((a)<(b)?(a):(b))
#define max(a,b) 				((a)>(b)?(a):(b))
#define abs(x) 					((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     			((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) 			((deg)*DEG_TO_RAD)
#define degrees(rad) 			((rad)*RAD_TO_DEG)
#define sq(x) 					((x)*(x))
 
// Bit-wise operations
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define _BV(bit) (1 << (bit))


#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))		
#define _NOP()  NOPS(1)
															
// to fix broken standart memset function
#define memset(p,v,s) _zme_memset(p,v,s); 
// Broken subroutines
// Memset was broken in stdlibrary of SDCC
void _zme_memset(byte * ptr, byte val, int count);


//**********************************************
void setup(void);
void loop(void);

//#if WITH_GENERAL_CHANNEL_VALUES
extern ZUNOGeneralChannelData_t g_channels_data[];
//#endif

