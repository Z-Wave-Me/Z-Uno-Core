	
#include "HardwareSerial.h"	
#include "ZUNO_Channels.h"

// All serial routines have similar offset from START fuction
// Define make code a little more readable
#define FUNC_START(num) 		(num)
#define FUNC_END(num) 			(num + 1)
#define FUNC_AVAILIABLE(num) 	(num + 2)
#define FUNC_READ(num) 			(num + 3)
#define FUNC_WRITE(num) 		(num + 4)

HardwareSerial::HardwareSerial(BYTE number) {
	func_vec = number;
}
void HardwareSerial::begin() {
	zunoSysCall(FUNC_START(func_vec), word(1152));
}
void HardwareSerial::begin(DWORD baudrate) {
	baudrate /= 100;
	zunoSysCall(FUNC_START(func_vec), word(baudrate));
}
void HardwareSerial::end() {
	zunoSysCall(FUNC_END(func_vec));
}
int HardwareSerial::available(void) {
	zunoSysCall(FUNC_AVAILIABLE(func_vec));
	return SYSRET_B;
}
int HardwareSerial::peek(void) {
	zunoSysCall(FUNC_AVAILIABLE(func_vec));
	if(!SYSRET_B)
		return -1;
	zunoSysCall(FUNC_READ(func_vec), byte(FALSE));
	return SYSRET_B;
}
int HardwareSerial::read(void) {
	zunoSysCall(FUNC_READ(func_vec), byte(TRUE));
	return SYSRET_B;
}
void HardwareSerial::flush(void){
}
size_t HardwareSerial::write(uint8_t value) {
	zunoSysCall(FUNC_WRITE(func_vec), value);
	return 1;
}

// Экземпляры классов - стиль как в Ардуино
HardwareSerial Serial(ZUNO_FUNC_SERIAL_USB_BEGIN);  // USB
HardwareSerial Serial1(ZUNO_FUNC_SERIAL1_BEGIN); // UART0
HardwareSerial Serial0(ZUNO_FUNC_SERIAL0_BEGIN); // UART1 -  Нужно его добавить, пока это тот же UART0