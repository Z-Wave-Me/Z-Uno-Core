#include "Core_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +
__code __at (0x8000) char lookupString[] = "LOOK UP ARDUINO HEX CODE";

SERIAL_DESCRIPTOR Serial;

void InitArduinoEnvironment(void);
void initSerial0(void);

ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(setterSwitchBinary,getterSwitchBinary),
                    ZUNO_SWITCH_MULTILEVEL(setterSwitchMultilevel,getterSwitchMultilevel),
                    ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_SMOKE,getterSensorBinary),
                    ZUNO_SENSOR_MULTILEVEL_TEMPERATURE(getterSensorMultilevel))


// DO NOT REMOVE THIS LINE - void other_functions_here(...)
void begin_setup_code(void) __naked {
    __asm
          .area ABSCODE (ABS,CODE)
          .org 0x8500        // YOUR FUNCTION'S DESIRED ADDRESS HERE.
    __endasm;
}
// DO NOT REMOVE THIS LINE - void setup(void) {}
void end_setup_code(void) __naked {
    __asm
        .area CSEG (REL,CODE)
    __endasm;
}

void begin_loop_code(void) __naked {
    __asm
      .area ABSCODE (ABS,CODE)
      .org 0x8750        // YOUR FUNCTION'S DESIRED ADDRESS HERE.
    __endasm;
}
// DO NOT REMOVE THIS LINE - void loop(void) {}
void end_loop_code(void) __naked {
    __asm
        .area CSEG (REL,CODE)
    __endasm;
}


// These functions will return to relative positions determined by the linker.
void main(void) {  
  InitArduinoEnvironment();
	setup();
  
	for (;;) {
		loop();
	}
}

void begin_init_code(void) __naked {
    __asm
          .area ABSCODE (ABS,CODE)
          .org 0x8300        // YOUR FUNCTION'S DESIRED ADDRESS HERE.
    __endasm;
}
void InitArduinoEnvironment(void) {
  initSerial0();
}

void end_init_code(void) __naked {
    __asm
        .area CSEG (REL,CODE)
    __endasm;
}
void initSerial0(void) {
  Serial.begin = &SerialBegin;
  Serial.end = &SerialEnd;
  Serial.available = &SerialAvailable;
  Serial.read = &SerialRead;
  Serial.write = &SerialWrite;
}




