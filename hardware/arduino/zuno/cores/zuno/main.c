#include "Core_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";

SERIAL_DESCRIPTOR Serial;

void InitArduinoEnvironment(void);
void initSerial0(void);

#if 0
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(setterSwitchBinary,getterSwitchBinary),
                    ZUNO_SWITCH_MULTILEVEL(setterSwitchMultilevel,getterSwitchMultilevel),
                    ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_SMOKE,getterSensorBinary),
                    ZUNO_SENSOR_MULTILEVEL_TEMPERATURE(getterSensorMultilevel))
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




