
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef void (*VOID_FUNC_POINTER_VOID) (void);
typedef void (*VOID_FUNC_POINTER_BYTE) (BYTE value);
typedef void (*VOID_FUNC_POINTER_DW0RD) (DWORD value);
typedef BYTE (*BYTE_FUNC_POINTER_VOID) (void);
typedef struct _SERIAL_DESCRIPTOR
{
	VOID_FUNC_POINTER_VOID begin;
	VOID_FUNC_POINTER_VOID 	end;
	BYTE_FUNC_POINTER_VOID 	available;
	BYTE_FUNC_POINTER_VOID 	read;
	VOID_FUNC_POINTER_BYTE 	write;
} SERIAL_DESCRIPTOR;


#define ZUNO_DELAY_SAFE_STACK_ADDRESS 				8052
#define ZUNO_DELAY_USER_STACK_POINTER_ADDRESS 		0x6E
//#define XBYTE  _xdata BYTE /* External data byte */

#define ZUNO_CORES_SW_VERSION_MAJOR 		0
#define ZUNO_CORES_SW_VERSION_MAJOR 		1 			

enum {
	ZUNO_FUNC_PIN_MODE,
	ZUNO_FUNC_DIGITAL_WRITE,
	ZUNO_FUNC_DIGITAL_READ,
	ZUNO_FUNC_ADD_SENSOR_BINARY_CHANNEL,
	ZUNO_FUNC_ADD_SENSOR_MULTILEVEL_CHANNEL,
	ZUNO_FUNC_ADD_METER_CHANNEL,
	ZUNO_FUNC_ADD_SWITCH_BINARY_CHANNEL,
	ZUNO_FUNC_ADD_SWITCH_MULTILEVEL_CHANNEL,
	ZUNO_FUNC_UNSOLICITED_REPORT,
	ZUNO_FUNC_ADD_ASSOCIATION_GROUP,
	ZUNO_FUNC_ASSOCIATION_SEND,
	ZUNO_FUNC_DELAY_MS,
	ZUNO_FUNC_ANALOG_READ,
	ZUNO_FUNC_ANALOG_WRITE,
	ZUNO_FUNC_SERIAL0_BEGIN,
	ZUNO_FUNC_SERIAL0_END,
	ZUNO_FUNC_SERIAL0_AVAILABLE,
	ZUNO_FUNC_SERIAL0_READ,
	ZUNO_FUNC_SERIAL0_WRITE,
};

enum {
	ZUNO_SENSOR_BINARY_GETTER,
	ZUNO_SENSOR_MULTILEVEL_GETTER,
	ZUNO_SWITCH_BINARY_GETTER,
	ZUNO_SWITCH_BINARY_SETTER,
	ZUNO_SWITCH_MULTILEVEL_GETTER,
	ZUNO_SWITCH_MULTILEVEL_SETTER,
	ZUNO_METER_GETTER,
};

#define ZUNO_SENSOR_BINARY_TYPE_GENERAL_PURPOSE 		0x01
#define ZUNO_SENSOR_BINARY_TYPE_SMOKE 					0x02
#define ZUNO_SENSOR_BINARY_TYPE_CO 						0x03
#define ZUNO_SENSOR_BINARY_TYPE_CO2 					0x04
#define ZUNO_SENSOR_BINARY_TYPE_HEAT 					0x05
#define ZUNO_SENSOR_BINARY_TYPE_WATER 					0x06
#define ZUNO_SENSOR_BINARY_TYPE_FREEZE 					0x07
#define ZUNO_SENSOR_BINARY_TYPE_TAMPER 					0x08
#define ZUNO_SENSOR_BINARY_TYPE_AUX 					0x09
#define ZUNO_SENSOR_BINARY_TYPE_DOOR_WINDOW 			0x0a
#define ZUNO_SENSOR_BINARY_TYPE_TILT 					0x0b
#define ZUNO_SENSOR_BINARY_TYPE_MOTION 					0x0c
#define ZUNO_SENSOR_BINARY_TYPE_GLASSBREAK 				0x0d
#define ZUNO_SENSOR_BINARY_TYPE_DEFAULT 				0xff

//Debug///
BYTE zunoPopByte(void);
void zunoPushByte(BYTE value);
void zunoPushWord(WORD value);
void zunoPushDWORD(DWORD value);
void zunoCall(void);
//********************************
void pinMode(BYTE pin, BYTE mode);
BYTE digitalRead(BYTE pin);
void digitalWrite(BYTE pin, BYTE value);
void delay(DWORD value);
WORD analogRead(BYTE pin);
void analogWrite(BYTE pin, BYTE value);
void SerialBegin(void);
void SerialEnd(void);
BYTE SerialAvailable(void);
BYTE SerialRead(void);
void SerialWrite(BYTE value);
BYTE zunoAddSensorBinaryChannel(BYTE type);
BYTE zunoAddSensorMultilevelChannel(BYTE type);
BYTE zunoAddSwitchBinaryChannel(void);
BYTE zunoAddSwitchMultilevelChannel(void);

void setterSwitchBinary(BYTE channel, BYTE value);
BYTE getterSwitchBinary(BYTE channel);
void setterSwitchMultilevel(BYTE channel, BYTE value);
BYTE getterSwitchMultilevel(BYTE channel);
BYTE getterSensorBinary(BYTE channel);
BYTE getterSensorMultilevel(BYTE channel);
void zunoSendUncolicitedReport(BYTE channel,BYTE value);

#define ZUNO_PIN_STATE_HIGH 				1
#define ZUNO_PIN_STATE_LOW 	 				0
#define ZUNO_SWITCHED_ON 					0xFF
#define ZUNO_SWITCHED_OFF 					0

#define HIGH 								ZUNO_PIN_STATE_HIGH
#define LOW 								ZUNO_PIN_STATE_LOW

#define ZUNO_STACK_SIZE 		50
#define ZUNO_STACK_ADDRESS 		8001
#define ZUNO_STACK_TOP_ADDRESS 	8000
#define ZUNO_DELAY_SAFE_STACK_ADDRESS 	8052

#define ZUNO_PIN_MODE_OUTPUT 				0
#define ZUNO_PIN_MODE_INPUT  				1
#define ZUNO_PIN_MODE_INPUT_WITH_PULLUP  	2
#define OUTPUT 								ZUNO_PIN_MODE_OUTPUT
#define INPUT 								ZUNO_PIN_MODE_INPUT

__sfr __at (0x81) SP;
