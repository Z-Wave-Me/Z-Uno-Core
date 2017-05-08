#include "LLCore_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";


void InitArduinoEnvironment(void);
void setup();
void loop();
 

//__sfr __at (0x81) SP;



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
  setup();
}




