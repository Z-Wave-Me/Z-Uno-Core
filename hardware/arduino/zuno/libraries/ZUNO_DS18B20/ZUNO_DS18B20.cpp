#include "ZUNO_DS18B20.h"
DS18B20Sensor::DS18B20Sensor(OneWire * ow):my_ow(ow)
{
}
byte DS18B20Sensor::scanAloneSensor(byte * rom)
{
	if(!my_ow->reset())
		return 0;
  	my_ow->readROM(rom);
  	if(rom[7] != my_ow->crc8(rom,7))
  		return 0;
  	return 1;
}
int DS18B20Sensor::getTempC100(byte * addr)
{
	int temp = BAD_TEMP;
	byte dallas_data[9];
	byte i;

	if (!my_ow->reset())
        return temp;
   
   	if(addr)
    	my_ow->select(addr);
    else
    	my_ow->skip();

    my_ow->write(0x44);

    delay(1000); 

    if(!my_ow->reset())
    	return temp;

    if(addr)
    	my_ow->select(addr);
    else
    	my_ow->skip();
        
    my_ow->write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) 
    {           // we need 9 bytes
      dallas_data[i] = my_ow->read();
    }

    temp = dallas_data[0];
    if(dallas_data[1] & 0x80)
    {
    	temp |= ((int) (dallas_data[1] & ~(0x80))) << 8;
    	temp -= temp;	
    }
    else
    {
    	temp |= ((int) dallas_data[1]) << 8;
    }

    

    return (temp * 25) >> 2;
}

float DS18B20Sensor::getTemperature(byte * addr)
{
    return getTempC100(addr) / 100.0;
       
}