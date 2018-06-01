#include "EEPROM.h"
//#include "ZUNO_call_proto.h"
#include "ZUNO_Channels.h"


#define WRITE_FUNC (rfunc + 1)
#define READ_FUNC (rfunc)

byte EEPROMClass::put(DWORD address, void * value, word val_size) {
    zunoSysCall(WRITE_FUNC, address, val_size, value);
    return SYSRET_B; 
}
byte EEPROMClass::get(DWORD address, void * value, word val_size) {
	zunoSysCall(READ_FUNC, address, val_size, value);
    return SYSRET_B;
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
	memcpy(&g_nzram_data[address], value, val_size);
    //zunoSysCall(WRITE_FUNC, address, val_size, value);
    return 1; 
}
byte NZRAMClass::get(byte address, void * value, byte val_size) {
    memcpy(value, &g_nzram_data[address], val_size);
	//zunoSysCall(READ_FUNC, address, val_size, value);
    return 1;
}
byte NZRAMClass::read(byte address) {
    return g_nzram_data[address];
}
void NZRAMClass::update(byte address, byte value) {
	//get(address, &temp_byte, 1);
    g_nzram_data[address] = value;
}
void NZRAMClass::write(byte address, byte value) {
	g_nzram_data[address] = value;
}

EEPROMClass EEPROM(ZUNO_FUNC_EEPROM_READ);
NZRAMClass NZRAM(ZUNO_FUNC_NZRAM_READ);
