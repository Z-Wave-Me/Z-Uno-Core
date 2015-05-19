#include "Core_arduino.h"

#define SEND_DEBUG(V)     \
	    __asm \
	    	MOV   R7,V \
	        LCALL 0x00FF40 \
	    __endasm;

__xdata __at (ZUNO_STACK_ADDRESS) unsigned char zunoStack[ZUNO_STACK_SIZE]; //
__xdata __at (ZUNO_STACK_TOP_ADDRESS) unsigned char zunoStackTop; //
__xdata __at (ZUNO_DELAY_SAFE_STACK_ADDRESS) unsigned char stack_pointer_outside;
__data __at (ZUNO_DELAY_USER_STACK_POINTER_ADDRESS) unsigned char user_stack_pointer;


void zunoPushByte(BYTE value) {
	if (zunoStackTop >= ZUNO_STACK_SIZE) {
		return;
	}
	zunoStack[++zunoStackTop] = value;
}

void zunoPushWord(WORD value) {
	if (zunoStackTop >= (ZUNO_STACK_SIZE - 1)) {
		return;
	}

	zunoStack[++zunoStackTop] = (BYTE)(value >> 8);
	zunoStack[++zunoStackTop] = (BYTE)(value & 0xFF);
}

void zunoPushDWORD(DWORD value) {
	if (zunoStackTop >= (ZUNO_STACK_SIZE - 3)) {
		return;
	}
	zunoStack[++zunoStackTop] = (BYTE)(value >> 24);
	zunoStack[++zunoStackTop] = (BYTE)(value >> 16);
	zunoStack[++zunoStackTop] = (BYTE)(value >> 8);
	zunoStack[++zunoStackTop] = (BYTE)(value & 0xFF);
}

BYTE zunoPopByte(void) {
	if (zunoStackTop == 0) {
		return 0;
	}

	return zunoStack[zunoStackTop--];
}

WORD zunoPopWord(void) {
	WORD dummy = 0;
	if (zunoStackTop <= 1) {
		return 0;
	}

	dummy |=zunoStack[zunoStackTop--];
	dummy |=(((WORD)zunoStack[zunoStackTop--])<<8);
	return dummy;
}

DWORD zunoPopDWORD(void) {
	DWORD dummy = 0;
	if (zunoStackTop <= 3) {
		return 0;
	}
	dummy |=zunoStack[zunoStackTop--];
	dummy |=(((DWORD)zunoStack[zunoStackTop--])<<8);
	dummy |=(((DWORD)zunoStack[zunoStackTop--])<<16);
	dummy |=(((DWORD)zunoStack[zunoStackTop--])<<24);
	return dummy;
}

void zunoCall(void) {
    __asm
          LCALL 0x002B00
    __endasm;
    /**/
}

/* ----------------------------------------------------------------------------
									GPIO
-------------------------------------------------------------------------------*/
void pinMode(BYTE pin, BYTE mode) {
	zunoPushByte(pin);
	zunoPushByte(mode);
	zunoPushByte(ZUNO_FUNC_PIN_MODE);
	zunoCall();
}

BYTE digitalRead(BYTE pin) {
	zunoPushByte(pin);
	zunoPushByte(ZUNO_FUNC_DIGITAL_READ);
	zunoCall();
	return zunoPopByte();
}


void digitalWrite(BYTE pin, BYTE value) {
	zunoPushByte(pin);
	zunoPushByte(value);
	zunoPushByte(ZUNO_FUNC_DIGITAL_WRITE);
	zunoCall();
}

WORD analogRead(BYTE pin) {
	zunoPushByte(pin);
	zunoPushByte(ZUNO_FUNC_ANALOG_READ);
	zunoCall();
	return zunoPopWord();
}

void analogWrite(BYTE pin, BYTE value) {
	zunoPushByte(pin);
	zunoPushByte(value);
	zunoPushByte(ZUNO_FUNC_ANALOG_WRITE);
	zunoCall();
}
/* ----------------------------------------------------------------------------
									GPIO
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
									Service
-------------------------------------------------------------------------------*/
void delay(DWORD value) {
	BYTE result;
	//TODO: check we are in loop

	//SaveTheSketchContent()
    __asm
        LCALL 0x00FF00
    __endasm;
    //

	zunoPushDWORD(value);
	zunoPushByte(ZUNO_FUNC_DELAY_MS);
	zunoCall();
	result = zunoPopByte();
	if (result != 0xFF)
	{
		user_stack_pointer = SP;
		SP = stack_pointer_outside; //is the next we store before entering
		return;
	} else {
		//TODO zuno error
	}
}

/* ----------------------------------------------------------------------------
									Service
-------------------------------------------------------------------------------*/



/* ----------------------------------------------------------------------------
									Serial
-------------------------------------------------------------------------------*/
//Serial (USB com)
void SerialBegin(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL0_BEGIN);
	zunoCall();
}

void SerialEnd(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL0_END);
	zunoCall();
}

BYTE SerialAvailable(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL0_AVAILABLE);
	zunoCall();
	return zunoPopByte();
}

BYTE SerialRead(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL0_READ);
	zunoCall();
	return zunoPopByte();
}

void SerialWrite(BYTE value) {
	zunoPushByte(value);
	zunoPushByte(ZUNO_FUNC_SERIAL0_WRITE);
	zunoCall();
}
//
/* ----------------------------------------------------------------------------
									Serial
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
									Z-Wave
Must be called in ArduinoInit function
-------------------------------------------------------------------------------*/
BYTE zunoAddSensorBinaryChannel(BYTE type) {
	zunoPushByte(type);
	zunoPushByte(ZUNO_FUNC_ADD_SENSOR_BINARY_CHANNEL);
	zunoCall();
	return zunoPopByte();
}

BYTE zunoAddSensorMultilevelChannel(BYTE type) {
	zunoPushByte(type);
	zunoPushByte(ZUNO_FUNC_ADD_SENSOR_MULTILEVEL_CHANNEL);
	zunoCall();
	return zunoPopByte();
}

BYTE zunoAddSwitchBinaryChannel(void) {
	zunoPushByte(ZUNO_FUNC_ADD_SWITCH_BINARY_CHANNEL);
	zunoCall();
	return zunoPopByte();
}

BYTE zunoAddSwitchMultilevelChannel(void) {
	zunoPushByte(ZUNO_FUNC_ADD_SWITCH_MULTILEVEL_CHANNEL);
	zunoCall();
	return zunoPopByte();
}

BYTE zunoAddAssociationGroup(BYTE type) {
	zunoPushByte(type);
	zunoPushByte(ZUNO_FUNC_ADD_ASSOCIATION_GROUP);
	zunoCall();
	return zunoPopByte();
}

/* ----------------------------------------------------------------------------
									Z-Wave
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
							Z-Wave communication
-------------------------------------------------------------------------------*/
void zunoSendUncolicitedReport(BYTE channel,BYTE value) {
	zunoPushByte(value);
	zunoPushByte(channel);
	zunoPushByte(ZUNO_FUNC_UNSOLICITED_REPORT);
	zunoCall();
}

/* ----------------------------------------------------------------------------
							Z-Wave communication
-------------------------------------------------------------------------------*/

void begin_callback_code(void) __naked {
    __asm
          .area ABSCODE (ABS,CODE)
          .org 0x8100        // YOUR FUNCTION'S DESIRED ADDRESS HERE.
    __endasm;
}

void zunoCallback(void) {
	BYTE channel = zunoPopByte();

	switch (zunoPopByte()) {
		case ZUNO_SENSOR_BINARY_GETTER:
			zunoPushByte(getterSensorBinary(channel));
			break;

		case ZUNO_SENSOR_MULTILEVEL_GETTER:
			zunoPushByte(getterSensorMultilevel(channel));
			break;

		case ZUNO_SWITCH_BINARY_GETTER:
			zunoPushByte(getterSwitchBinary(channel));
			break;

		case ZUNO_SWITCH_BINARY_SETTER:
			setterSwitchBinary(channel, zunoPopByte());
			break;

		case ZUNO_SWITCH_MULTILEVEL_GETTER:
			zunoPushByte(getterSwitchMultilevel(channel));
			break;

		case ZUNO_SWITCH_MULTILEVEL_SETTER:
			setterSwitchMultilevel(channel, zunoPopByte());
			break;
		default:
			break;
	}
}

void end_callback_code(void) __naked {
    __asm
        .area CSEG (REL,CODE)
    __endasm;
}