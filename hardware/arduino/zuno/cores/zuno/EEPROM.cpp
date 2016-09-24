#include "EEPROM.h"
#include "ZUNO_call_proto.h"

#define WRITE_FUNC (rfunc + 1)
#define READ_FUNC (rfunc)


word EEPROMClass::put(DWORD address, void * value, word val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	zunoPushWord(val_size);
	zunoPushDword(address);
	zunoPushByte(WRITE_FUNC);
    zunoCall();	
    return zunoPopWord(); 
}
word EEPROMClass::get(DWORD address, void * value, word val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	zunoPushWord(val_size);
	zunoPushDword(address);
	zunoPushByte(READ_FUNC);
    zunoCall();	
    return zunoPopWord();
}
byte EEPROMClass::read(DWORD address)
{
	get(address, &temp_byte, 1);
    return temp_byte;
}
void EEPROMClass::update(DWORD address, byte value)
{
	get(address, &temp_byte, 1);

    if(value != temp_byte)
    {
    	temp_byte = value;	
		put(address, &temp_byte, 1);
    }

}
void EEPROMClass::write(word address, byte value)
{
	temp_byte = value;	
	put(address, &temp_byte, 1);

}

byte NZRAMClass::put(byte address, void * value, byte val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	zunoPushByte(val_size);
	zunoPushByte(address);
	zunoPushByte(WRITE_FUNC);
    zunoCall();	
    return zunoPopByte(); 
}
byte NZRAMClass::get(byte address, void * value, byte val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	zunoPushByte(val_size);
	zunoPushByte(address);
	zunoPushByte(READ_FUNC);
    zunoCall();	
    return zunoPopByte();
}
byte NZRAMClass::read(byte address)
{
	get(address, &temp_byte, 1);
    return temp_byte;
}
void NZRAMClass::update(byte address, byte value)
{
	get(address, &temp_byte, 1);

    if(value != temp_byte)
    {
    	temp_byte = value;	
		put(address, &temp_byte, 1);
    }

}
void NZRAMClass::write(byte address, byte value)
{
	temp_byte = value;	
	put(address, &temp_byte, 1);
}

EEPROMClass EEPROM(ZUNO_FUNC_EEPROM_READ);
NZRAMClass NZRAM(ZUNO_FUNC_NZRAM_READ);
