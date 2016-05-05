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

#endif // ARDUINO_ZUNO_C_API
