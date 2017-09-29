#include "EEPROM.h"
//#include "ZUNO_call_proto.h"
#include "ZUNO_Channels.h"

#define WRITE_FUNC (rfunc + 1)
#define READ_FUNC (rfunc)

word EEPROMClass::put(DWORD address, void * value, word val_size) {
    zunoSysCall(WRITE_FUNC, address, val_size, value);
    return SYSRET_W; 
}
word EEPROMClass::get(DWORD address, void * value, word val_size) {
	zunoSysCall(READ_FUNC, address, val_size, value);
    return SYSRET_W;
}
byte EEPROMClass::read(DWORD address) {
	get(address, &temp_byte, 1);
    return temp_byte;
}
void EEPROMClass::update(DWORD address, byte value) {
	get(address, &temp_byte, 1);
    if(value != temp_byte) {
    	temp_byte = value;	
		put(address, &temp_byte, 1);
    }
}
void EEPROMClass::write(DWORD address, byte value) {
	temp_byte = value;	
	put(address, &temp_byte, 1);

}
byte NZRAMClass::put(byte address, void * value, byte val_size) {
	zunoSysCall(WRITE_FUNC, address, val_size, value);
    return SYSRET_B; 
}
byte NZRAMClass::get(byte address, void * value, byte val_size) {
	zunoSysCall(READ_FUNC, address, val_size, value);
    return SYSRET_B;
}
byte NZRAMClass::read(byte address) {
	get(address, &temp_byte, 1);
    return temp_byte;
}
void NZRAMClass::update(byte address, byte value) {
	get(address, &temp_byte, 1);
    if(value != temp_byte) {
    	temp_byte = value;	
		put(address, &temp_byte, 1);
    }
}
void NZRAMClass::write(byte address, byte value) {
	temp_byte = value;	
	put(address, &temp_byte, 1);
}

EEPROMClass EEPROM(ZUNO_FUNC_EEPROM_READ);
NZRAMClass NZRAM(ZUNO_FUNC_NZRAM_READ);
