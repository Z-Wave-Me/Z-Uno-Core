#ifndef ZUNO_EEPROM_LIBRARY
#define ZUNO_EEPROM_LIBRARY

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

class EEPROMClass
{
	public:
		EEPROMClass(byte read_func):rfunc(read_func){};

	
		word put(DWORD address, void * value, word val_size);
		word get(DWORD address, void * value, word val_size);
		byte read(DWORD address);
		void update(DWORD address, byte value);
		void write(DWORD address, byte value);
		
	private:
		byte temp_byte;
		byte rfunc;
		

};
class NZRAMClass
{
	public:
		NZRAMClass(byte read_func):rfunc(read_func){};

		byte put(byte address, void * value, byte val_size);
		byte get(byte address, void * value, byte val_size);
		byte read(byte address);
		void update(byte address, byte value);
		void write(byte address, byte value);
		
	private:

		byte temp_byte;
		byte rfunc;
		
};
extern EEPROMClass EEPROM;
extern NZRAMClass NZRAM;

#endif // ZUNO_EEPROM_LIBRARY