#include "Core_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";

SERIAL_DESCRIPTOR Serial;
SERIAL_1_DESCRIPTOR Serial1;

void InitArduinoEnvironment(void);
void initSerial0(void);
void initSerial1(void);

#if 0
__code ZUNO_ASSOCIATION_PROPERTIES_DESCRIPTION zunoAssociationSetupArray[]= 
                { 
                  {0x42, 0x42},
                  ZUNO_ASSOCIATION_GROUP_SET_VALUE,
                  ZUNO_ASSOCIATION_GROUP_SET_VALUE_AND_DIM,
                  ZUNO_ASSOCIATION_GROUP_SCENE_CONTROL,
                  {0x43, 0x43} 
                };
#endif



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

void InitArduinoEnvironment(void) {
  //TODO add initialization call C51STARTUP
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
}




