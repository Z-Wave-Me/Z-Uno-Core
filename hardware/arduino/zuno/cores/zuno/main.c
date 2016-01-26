#include "Core_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";

SERIAL_DESCRIPTOR Serial;
SERIAL_1_DESCRIPTOR Serial1;

void InitArduinoEnvironment(void);
void initSerial0(void);
void initSerial1(void);


// DO NOT REMOVE THIS LINE - void other_functions_here(...)

// DO NOT REMOVE THIS LINE - void setup(void) {}

// DO NOT REMOVE THIS LINE - void loop(void) {}


// These functions will return to relative positions determined by the linker.
void main(void) {  
  InitArduinoEnvironment();
  
	for (;;) {
		loop();
	}
}

void xdata8051_init(void) {
  __asm
  ;
  ; check there is any user xdata
  MOV r0,#l_XINIT
  MOV a,r0
  ORL a,#(l_XINIT >> 8)
  JZ  EXIT
  ;
  ;
  ;
  ; load registers
  ; r0-1
  MOV r1,#((l_XINIT+255) >> 8)
  MOV r2,#s_XINIT
  MOV r3,#(s_XINIT >> 8)
  MOV r4,#s_XISEG
  MOV r5,#(s_XISEG >> 8)
CYCLE: clr a
  MOV dpl,r2
  MOV dph,r3
  MOVc  a,@a+dptr
  INC dptr
  MOV r2, dpl
  MOV r3, dph
  MOV dpl,r4
  MOV dph,r5
  MOVX  @dptr,a
  INC dptr
  MOV r4,dpl
  MOV r5,dph
  DJNZ  r0,CYCLE
  DJNZ  r1,CYCLE
EXIT:
  __endasm;
}

void InitArduinoEnvironment(void) {
  xdata8051_init();
  initSerial0();
  initSerial1();
  setup();
}

void initSerial0(void) {
  Serial.begin = &SerialBegin;
  Serial.end = &SerialEnd;
  Serial.available = &SerialAvailable;
  Serial.read = &SerialRead;
  Serial.write = &SerialWrite;
  Serial.println = &SerialPrintln;
  Serial.print = &SerialPrint;
  Serial.printChar = &SerialPrint_char;
}

void initSerial1(void) {
  Serial1.begin = &Serial1_Begin;
  Serial1.end = &Serial1_End;
  Serial1.available = &Serial1_Available;
  Serial1.read = &Serial1_Read;
  Serial1.write = &Serial1_Write;
  Serial1.println = &Serial1_Println;
  Serial1.print = &Serial1_Print;
  Serial1.printChar = &Serial1_Print_char;
}




