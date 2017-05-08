#ifndef ARDUINO_ZUNO_C_API
#define ARDUINO_ZUNO_C_API

#include "ArduinoTypes.h"
// Интерфейс низкого уровня, который доступен пользователю
// Прототипы из LLCore_arduino.c
void pinMode(BYTE pin, BYTE mode);
BYTE digitalRead(BYTE pin);
void digitalWrite(BYTE pin, BYTE value);
void delay(DWORD value);
WORD analogRead(BYTE pin);
void analogWrite(BYTE pin, BYTE value);
DWORD millis();

// WakeUp on KS
void zunoSetupKeyScannerWU(byte cols_num);
// Z-Uno Systems calls
void zunoSendUncolicitedReport(BYTE channel,WORD value);
void zunoSendAssociationCommand(BYTE group, BYTE assoc_type, BYTE param1, BYTE param2);
void zunoSendDeviceToSleep(void);
BYTE zunoGetWakeReason(void);

void delayLoops(byte v);
void NOPS(byte i);

// interrupts
void noInterrupts();
void interrupts();

// math
long map(long x, long in_min, long in_max, long out_min, long out_max);




#endif // ARDUINO_ZUNO_C_API
