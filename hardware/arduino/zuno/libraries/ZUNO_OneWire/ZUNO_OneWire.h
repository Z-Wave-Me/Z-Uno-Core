#ifndef ZUNO_ONEWIRE_LIB
#define ZUNO_ONEWIRE_LIB

#include "Arduino.h"
class OneWire
{
public:
	OneWire(s_pin bus_pin);

	byte reset(void);
	void write(byte v, byte power = 1); 
	byte read(); 
	void select(byte * rom);
	void skip();
	void depower();
	void readROM(byte * rom);
	byte crc8(byte *addr, byte len);

	void reset_search();
	bool search(uint8_t *newAddr);



private:
	s_pin bus_pin;	

	byte  LastDiscrepancy;
	byte  LastFamilyDiscrepancy;
	bool  LastDeviceFlag; 
	bool  read_bit();
	void  write_bit(bool bit);


};

#endif /// ZUNO_ONEWIRE_LIB