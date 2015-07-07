#include "Core_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";

SERIAL_DESCRIPTOR Serial;

void InitArduinoEnvironment(void);
void initSerial0(void);

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
  setup();
}

void initSerial0(void) {
  Serial.begin = &SerialBegin;
  Serial.end = &SerialEnd;
  Serial.available = &SerialAvailable;
  Serial.read = &SerialRead;
  Serial.write = &SerialWrite;
}




