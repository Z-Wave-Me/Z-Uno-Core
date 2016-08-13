#include "EEPROM.h"
#include "ZUNO_call_proto.h"

#define WRITE_FUNC (rfunc + 1)
#define READ_FUNC (rfunc)


word EEPROMClass::put(word address, void * value, word val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	pushAddresses(address, val_size);
	zunoPushByte(WRITE_FUNC);
    zunoCall();	
    return popResult(); 
}
word EEPROMClass::get(word address, void * value, word val_size)
{
	zunoPushWord(reinterpPOINTER((byte*)value));
	pushAddresses(address, val_size);
	zunoPushByte(READ_FUNC);
    zunoCall();	
    return popResult();
}
byte EEPROMClass::read(word address)
{
	zunoPushWord(reinterpPOINTER(&temp_byte));
	pushAddresses(address, 1);
	zunoPushByte(READ_FUNC);
    zunoCall();	
    popResult();

    return temp_byte;
}
void EEPROMClass::update(word address, byte value)
{
	zunoPushWord(reinterpPOINTER(&temp_byte));
	pushAddresses(address, 1);
	zunoPushByte(READ_FUNC);
    zunoCall();	
    popResult();

    if(value != temp_byte)
    {
    	temp_byte = value;	
		zunoPushWord(reinterpPOINTER(&temp_byte));
		pushAddresses(address, 1);
		zunoPushByte(WRITE_FUNC);
    	zunoCall();	
    	popResult();
    }

}
void EEPROMClass::write(word address, byte value)
{
	temp_byte = value;	
	zunoPushWord(reinterpPOINTER(&temp_byte));
	pushAddresses(address, 1);
	zunoPushByte(WRITE_FUNC);
    zunoCall();	
    popResult();

}
void EEPROMClass::pushAddresses(word address,  word val_size)
{
	if(addrsize == 2)
	{
		zunoPushWord(val_size);
		zunoPushWord(address);
	}
	else
	{
		zunoPushByte(val_size & 0xFF);
		zunoPushByte(address & 0xFF);	
	}
}

word EEPROMClass::popResult()
{
	if(addrsize == 2)
		return zunoPopWord();
	return zunoPopByte();
}

EEPROMClass EEPROM(ZUNO_FUNC_EEPROM_READ, 2);
EEPROMClass NZRAM(ZUNO_FUNC_NZRAM_READ, 1);
