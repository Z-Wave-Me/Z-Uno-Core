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
void delay(DWORD value);
void delayLoops(byte v);
//**********************************************
void setup(void);
void loop(void);
void InitArduinoEnvironment(void);

// System low-level types...
typedef struct _ZUNO_ISR_DESCRIPTION
{
	BYTE insr_num;
	GENERIC_POINTER handler;
} ZUNO_ISR_DESCRIPTION;

/************************************************
			end of Variables
************************************************/


#endif // CORE_ARDUINO_ZUNO__


