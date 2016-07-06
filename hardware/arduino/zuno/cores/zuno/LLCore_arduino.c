#include "LLCore_arduino.h"


__xdata __at (ZUNO_STACK_ADDRESS) unsigned char zunoStack[ZUNO_STACK_SIZE]; //
__xdata __at (ZUNO_STACK_TOP_ADDRESS) unsigned char zunoStackTop; //
__xdata __at (ZUNO_DELAY_SAFE_STACK_ADDRESS) unsigned char stack_pointer_outside;
__data __at (ZUNO_DELAY_USER_STACK_POINTER_ADDRESS) unsigned char user_stack_pointer;


#include "Custom.h"


void Serial1_Print_char(BYTE value);
void Serial1_Print(const char* bufPointer, BYTE size);
void Serial1_Println(const char* bufPointer);

__sfr __at (0x81) SP;



// Дополнительный код

BYTE zme_strlen(char * str)
{
	BYTE i = 0;
	while(str[i] != 0)
		i++;
	return i;
}
//


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

void delayMicroseconds(unsigned int value) {
    __asm
    mov	r6,dpl
    mov	r7,dph
LOOP:    nop
    nop
    nop
    nop
    nop
    nop
    djnz r6,LOOP
    cjne r7,#0x00,CONTINUE
EXIT:    ret
CONTINUE:    djnz r7,LOOP
    __endasm;
}

DWORD millis(void) {
	zunoPushByte(ZUNO_FUNC_MILLIS);
	zunoCall();
	return zunoPopDWORD();
}

void zunoSendDeviceToSleep(void) {
	zunoPushByte(ZUNO_FUNC_GO_SLEEP);
	zunoCall();
}

BYTE zunoGetWakeReason(void) {
	zunoPushByte(ZUNO_FUNC_GET_WAKE_UP_REASON);
	zunoCall();
	return zunoPopByte();
}

/* ----------------------------------------------------------------------------
									Service
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
									SPI
-------------------------------------------------------------------------------*/
void zunoSpi0Init(BYTE speed, BYTE mode, BYTE border) {

	if 	(  (speed > SPI_SPEED_1_MHZ)
		|| (mode > SPI_MODE3)
		|| (border > MSBFIRST)) {
		// unsupported values
		return;
	}
	zunoPushByte(speed | (mode * 4) | (border * 0x10));
	zunoPushByte(ZUNO_FUNC_SPI0_INIT);
	zunoCall();
}

void zunoSpi0Begin(void) {
	zunoPushByte(TRUE);
	zunoPushByte(ZUNO_FUNC_SPI0_ENABLE);
	zunoCall();
}

void zunoSpi0End(void) {
	zunoPushByte(FALSE);
	zunoPushByte(ZUNO_FUNC_SPI0_ENABLE);
	zunoCall();
}

BYTE zunoSpi0Transfer(BYTE value) {
	zunoPushByte(value);
	zunoPushByte(ZUNO_FUNC_SPI0_TRANSFER);
	zunoCall();
	return zunoPopByte();
}
/* ----------------------------------------------------------------------------
									SPI
-------------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------
									I2C
-------------------------------------------------------------------------------*/
#define SCL_PIN  9
#define SDA_PIN  10
void zunoI2CInit()
{
	// Инициализируем пины
    pinMode(SCL_PIN, OUTPUT);
    pinMode(SDA_PIN, OUTPUT);
    digitalWrite(SCL_PIN, HIGH);
    digitalWrite(SDA_PIN, HIGH);
}
void zunoI2CBegin()
{
	// Подсоединяемся к шине I2C как мастер
    zunoPushByte(ZUNO_FUNC_I2C_BEGIN);
    zunoCall();
}
void zunoI2CEnd()
{
	zunoPushByte(ZUNO_FUNC_I2C_END);
    zunoCall();
}
void zunoI2CWrite(BYTE data)
{
	zunoPushByte(data);
	zunoPushByte(ZUNO_FUNC_I2C_WRITE);
    zunoCall();
}
BYTE zunoI2CRead()
{
	zunoPushByte(ZUNO_FUNC_I2C_READ);
    zunoCall();		
	return zunoPopByte();
}
/* ----------------------------------------------------------------------------
									I2C
-------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------
									DHT
-------------------------------------------------------------------------------*/

BYTE zunoDHTreadSensor(BYTE * array, BYTE pin, BYTE wakeupDelay, BYTE leadingZeroBits) {
	BYTE i = 0;
	BYTE status;


    // REQUEST SAMPLE
	pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // T-be
    delay(wakeupDelay);
    //start data receiving
	zunoPushByte(leadingZeroBits);
	zunoPushByte(wakeupDelay);
	zunoPushByte(pin);
	zunoPushByte(ZUNO_FUNC_DHT_READ_SENSOR);
    zunoCall();
    status = zunoPopByte();
    // 0 is reserved value, so midlleware will return "TRUE" if everything is ok
    if (status == TRUE) {
    	//everything is ok
	    for (; i < 5; i++) {
	    	array[i] = zunoPopByte();
	    }
	    //fallback to original Lib value
	    status = 0;
	}

	if (status == 0) {
		status = -6;
		// some internal problems
	}
	
	return status;
}
/* ----------------------------------------------------------------------------
									DHT
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
