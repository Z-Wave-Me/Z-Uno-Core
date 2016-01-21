#include "Core_arduino.h"

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

void zunoPushDword(DWORD value) {
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

	zunoPushDword(value);
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

void zunoSendDeviceToSleep(void) {
	zunoPushByte(ZUNO_FUNC_GO_SLEEP);
	zunoCall();
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

void SerialPrintln(const char* bufPointer) {
	BYTE i = 0;
	while ((bufPointer[i] != 0) && (i < 100)) {
		SerialWrite(bufPointer[i]);
		i++;
	}
	SerialWrite('\n');
}

void SerialPrint(const char* bufPointer, BYTE size) {
	BYTE i = 0;
	while (i < size) {
		SerialWrite(bufPointer[i]);
		i++;
	}
}

void SerialPrint_char(BYTE value) {
	if (value >= 100) {
  		SerialWrite((value / 100) + 0x30);
  	}
	if (value >= 10) {
  		SerialWrite(((value / 10) % 10) + 0x30);
	}
	SerialWrite((value % 10) + 0x30);
}

/* ----------------------------------------------------------------------------
									Serial
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
									Serial1
-------------------------------------------------------------------------------*/
//Serial (UART)
void Serial1_Begin(DWORD baudrate) {
	WORD correct_dbrt = (WORD)(baudrate / 100);
	zunoPushWord(correct_dbrt);
	zunoPushByte(ZUNO_FUNC_SERIAL1_BEGIN);
	zunoCall();
}

void Serial1_End(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL1_END);
	zunoCall();
}

BYTE Serial1_Available(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL1_AVAILABLE);
	zunoCall();
	return zunoPopByte();
}

BYTE Serial1_Read(void) {
	zunoPushByte(ZUNO_FUNC_SERIAL1_READ);
	zunoCall();
	return zunoPopByte();
}

void Serial1_Write(BYTE value) {
	zunoPushByte(value);
	zunoPushByte(ZUNO_FUNC_SERIAL1_WRITE);
	zunoCall();
}

void Serial1_Println(const char* bufPointer) {
	BYTE i = 0;
	while ((bufPointer[i] != 0) && (i < 100)) {
		Serial1_Write(bufPointer[i]);
		i++;
	}
	Serial1_Write('\n');
}

void Serial1_Print(const char* bufPointer, BYTE size) {
	BYTE i = 0;
	while (i < size) {
		Serial1_Write(bufPointer[i]);
		i++;
	}
}

void Serial1_Print_char(BYTE value) {
	if (value >= 100) {
      	Serial1_Write((value / 100) + 0x30);
	}

	if (value >= 10) {
  		Serial1_Write(((value / 10) % 10) + 0x30);
	}
	Serial1_Write((value % 10) + 0x30);
}
//
/* ----------------------------------------------------------------------------
									Serial1
-------------------------------------------------------------------------------*/



/* ----------------------------------------------------------------------------
							Z-Wave communication
-------------------------------------------------------------------------------*/
void zunoSendUncolicitedReport(BYTE channel,WORD value) {
	zunoPushWord(value);
	zunoPushByte(channel);
	zunoPushByte(ZUNO_FUNC_UNSOLICITED_REPORT);
	zunoCall();
}

void zunoSendAssociationCommand(BYTE group, BYTE assoc_type, BYTE param1, BYTE param2) {
	zunoPushByte(param2);
	zunoPushByte(param1);
	zunoPushByte(assoc_type);
	zunoPushByte(group);
	zunoPushByte(ZUNO_FUNC_ASSOCIATION_SEND);
	zunoCall();
}


void zunoCallback(void) {
	BYTE channel_and_cmd_type = zunoPopByte();
	BYTE channel = channel_and_cmd_type / 2;

	if ((channel_and_cmd_type % 2) == 0) { //we got a getter
		BYTE size = zunoPopByte();
		if (channel_and_cmd_type == 0) {
			return;
		}
		if (channel == 3) {
			digitalWrite(5,HIGH);
		} else if (channel == 2) {
			digitalWrite(5,LOW);
		}
		//no shift, vecause channels start from 1 both in Z-Wave and in our storage array
		if ((zunoChannelSetupArray[channel].getter != NULL) && (channel_and_cmd_type != 0)) {
			switch(size) {
				case 1:
				{
					BYTE ret_value = (*((BYTE_FUNC_POINTER_VOID)zunoChannelSetupArray[channel].getter))();
					zunoPushByte(ret_value);
					break;
				}

				case 2:
				{
					WORD ret_value = (*((WORD_FUNC_POINTER_VOID)zunoChannelSetupArray[channel].getter))();
					zunoPushWord(ret_value);
					break;
				}

				case 4:
				{
					DWORD ret_value = (*((DWORD_FUNC_POINTER_VOID)zunoChannelSetupArray[channel].getter))();
					zunoPushDword(ret_value);
					break;
				}
			}
		} else {
			zunoPushByte(0);
		}
	} else { //we got a setter
		BYTE value = zunoPopByte();
		//TODO maybe we have additional parameters
		//no shift, vecause channels start from 1 both in Z-Wave and in our storage array
		if ((zunoChannelSetupArray[channel].setter != NULL) && (channel_and_cmd_type != 1)) {
			(*((VOID_FUNC_POINTER_BYTE)zunoChannelSetupArray[channel].setter))(value);
		}
	}
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

void zunoJumpTable(void) {
	BYTE requested_function = zunoPopByte();
	switch(requested_function) {
		case ZUNO_JUMP_TABLE_SETUP:
		InitArduinoEnvironment();
		break;

		case ZUNO_JUMP_TABLE_LOOP:
		loop();
		break;

		case ZUNO_JUMP_TABLE_CALLBACK:
		zunoCallback();
		break;

		case ZUNO_GET_CHANNELS_ADDRESS:
		{
			BYTE __code * p_code_space = (BYTE __code *) zunoChannelSetupArray;
			zunoPushWord((WORD)p_code_space);
		}
		break; 

		case ZUNO_GET_ASSOCIATIONS_ADDRESS:
		{
			BYTE __code * p_code_space = (BYTE __code *) zunoAssociationSetupArray;
			zunoPushWord((WORD)p_code_space);
		}
		break;

		case ZUNO_GET_SLEEPING_MODE:
		zunoPushByte(zunoSleepingModeSetupStruct.current_mode);
		break;

		case ZUNO_GET_DEBUG_MODE_PARAM:
		zunoPushByte(zunoDebugParameter);
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
