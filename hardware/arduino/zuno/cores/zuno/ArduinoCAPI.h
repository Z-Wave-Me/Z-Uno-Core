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


// SPI
void zunoSpi0Init(BYTE speed, BYTE mode, BYTE border);
void zunoSpi0Begin(void);
void zunoSpi0End(void);
BYTE zunoSpi0Transfer(BYTE value);

// I2C
void zunoI2CInit();
void zunoI2CBegin();
void zunoI2CEnd();
void zunoI2CWrite(BYTE data);
BYTE zunoI2CRead();

//DHT
BYTE zunoDHTreadSensor(BYTE * array, BYTE pin, BYTE wakeupDelay);

// EEPROM
WORD zunoReadEEPROM(WORD address, WORD size, BYTE * destination);
WORD zunoWriteEEPROM(WORD address, WORD size, BYTE * source);
// WakeUp on KS
void zunoSetupKeyScannerWU(byte cols_num);
// Z-Uno Systems calls
void zunoSendUncolicitedReport(BYTE channel,WORD value);
void zunoSendAssociationCommand(BYTE group, BYTE assoc_type, BYTE param1, BYTE param2);
void zunoSendDeviceToSleep(void);
BYTE zunoGetWakeReason(void);

void delayMicroseconds(unsigned int value);

// interrupts
void noInterrupts();
void interrupts();



#endif // ARDUINO_ZUNO_C_API
