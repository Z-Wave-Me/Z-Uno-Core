#ifndef ARDUINO_ZUNO_C_API
#define ARDUINO_ZUNO_C_API

#include "ArduinoTypes.h"
// Интерфейс низкого уровня, который доступен пользователю
// Прототипы из LLCore_arduino.c
void pinMode(BYTE pin, BYTE mode);
BYTE digitalRead(BYTE pin);
void digitalWrite(BYTE pin, BYTE value);
void digitalToggle(byte pin);
void delay(DWORD value);
WORD analogRead(BYTE pin);
void analogWrite(BYTE pin, WORD value);
DWORD millis();

void analogReference(BYTE ref);
void analogReadResolution(BYTE bits);
void analogWriteResolution(BYTE bits);

void zunoADCAdvConfig(BYTE flags);
// WakeUp on KS
void zunoSetupKeyScannerWU(byte cols_num);
// Z-Uno Systems calls
void zunoSendUncolicitedReport(BYTE channel,WORD value);
void zunoSendAssociationCommand(BYTE group, BYTE assoc_type, BYTE param1, BYTE param2);
void zunoSendDeviceToSleep(void);
BYTE zunoGetWakeReason(void);

void delayLoops(byte v);
void NOPS(byte i);

void zunoExtIntMode(byte inum, byte mode);
void zunoGPTInit(byte flags);
void zunoGPTEnable(byte bEnable);
void zunoGPTSet(word interval);


// interrupts
void noInterrupts();
void interrupts();

// math
long map(long x, long in_min, long in_max, long out_min, long out_max);


void zunoAddChannel(byte type, byte st, byte p);
void zunoAddAssociation(byte t);
void zunoCommitConfig();
void zunoStartLearn(BYTE timeout);


#endif // ARDUINO_ZUNO_C_API
