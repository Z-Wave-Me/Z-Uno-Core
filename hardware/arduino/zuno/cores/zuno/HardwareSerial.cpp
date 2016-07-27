	
#include "HardwareSerial.h"	
#include "ZUNO_Definitions.h"
#include "ZUNO_call_proto.h"

// Чтобы было проще настраивать потом - определяем смещения от первого enum
#define FUNC_START(num) 		(num)
#define FUNC_END(num) 			(num + 1)
#define FUNC_AVAILIABLE(num) 	(num + 2)
#define FUNC_READ(num) 			(num + 3)
#define FUNC_WRITE(num) 		(num + 4)



HardwareSerial::HardwareSerial(BYTE number)
{
	func_vec = number;
}


void HardwareSerial::begin()
{
	zunoPushWord(1152);  //default baudrate is 115200
	zunoPushByte(FUNC_START(func_vec));
	zunoCall();
}
    	
void HardwareSerial::begin(DWORD baudrate)
{
	WORD correct_dbrt = (WORD)(baudrate / 100);
	zunoPushWord(correct_dbrt);
	zunoPushByte(FUNC_START(func_vec));
	zunoCall();
}
void HardwareSerial::end()
{
	zunoPushByte(FUNC_END(func_vec));
	zunoCall();
}
int HardwareSerial::available(void)
{
	zunoPushByte(FUNC_AVAILIABLE(func_vec));
	zunoCall();
	return zunoPopByte();
}
int HardwareSerial::peek(void)
{
	byte res  = 0;
	zunoPushByte(FUNC_AVAILIABLE(func_vec));
	zunoCall();
	res = zunoPopByte();
	if(!res)
		return -1;
	zunoPushByte(FALSE); // not to remove byte from read buffer
	zunoPushByte(FUNC_READ(func_vec));
	zunoCall();
	return zunoPopByte();
}
int HardwareSerial::read(void)
{
	zunoPushByte(TRUE); // remove byte from read buffer
	zunoPushByte(FUNC_READ(func_vec));
	zunoCall();
	return zunoPopByte();
}
void HardwareSerial::flush(void)
{

}
size_t HardwareSerial::write(uint8_t value)
{

	zunoPushByte(value);
	zunoPushByte(FUNC_WRITE(func_vec));
	zunoCall();
	// Несоотвествие ардуиновскому вызову
	// должно быть 
	// return zunoPopByte();
	// которое возвращает либо 0 - байт не записан, либо 1 - байт записан
	return 1;
}

// Экземпляры классов - стиль как в Ардуино
HardwareSerial Serial(ZUNO_FUNC_SERIAL0_BEGIN);  // USB
HardwareSerial Serial1(ZUNO_FUNC_SERIAL1_BEGIN); // UART0
HardwareSerial Serial2(ZUNO_FUNC_SERIAL1_BEGIN); // UART1 -  Нужно его добавить, пока это тот же UART0