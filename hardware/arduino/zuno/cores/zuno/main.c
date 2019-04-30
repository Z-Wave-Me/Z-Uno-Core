#include "LLCore_arduino.h"
#pragma codeseg BANK3
#pragma preproc_asm +

// Signature
__code __at (0x8000) char lookupString[] = {'Z','M','E','Z','U','N','O', ZUNO_CORES_SW_VERSION_MAJOR, ZUNO_CORES_SW_VERSION_MINOR};
void InitArduinoEnvironment(void);
void setup();
void loop();
// These functions will return to relative positions determined by the linker.
void main(void) {  
  return;
}






