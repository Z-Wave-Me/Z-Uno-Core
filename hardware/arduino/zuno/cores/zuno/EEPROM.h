#ifndef ZUNO_EEPROM_LIBRARY
#define ZUNO_EEPROM_LIBRARY

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

class EEPROMClass
{
	public:
		EEPROMClass(byte read_func, byte addr_size){rfunc = read_func;  addrsize = addr_size; };

	
		word put(word address, void * value, word val_size);
		word get(word address, void * value, word val_size);
		byte read(word address);
		void update(word address, byte value);
		void write(word address, byte value);
		
	private:

		void pushAddresses(word address,  word val_size);
		word popResult();
		byte temp_byte;
		byte rfunc;
		byte addrsize;

};
extern EEPROMClass EEPROM;
extern EEPROMClass NZRAM;

#endif // ZUNO_EEPROM_LIBRARY