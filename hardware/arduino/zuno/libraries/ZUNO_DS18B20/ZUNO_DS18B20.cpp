#include "ZUNO_DS18B20.h"
#define DS18B20FAMILY 0x28

#define DEBUG_TEMP_CONV  0
DS18B20Sensor::DS18B20Sensor(OneWire * ow):
        my_ow(ow),
        current_resolution(DS18B20_RESOLUTION_12BIT),
        current_delay(75) // in 10th of microseconds == 750ms
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


byte DS18B20Sensor::findAllSensors(byte * rom)
{
    byte count = 0;
    my_ow->reset_search();
    while(my_ow->search(rom))
    {
        // Check that CRC is valid
        if(rom[7] != my_ow->crc8(rom,7))
            continue;
        // Check that it's a ds18b20 sensor
        if(rom[0] != DS18B20FAMILY)
            continue;
        rom += 8;
        count++;
    }

    return count;

}
void DS18B20Sensor::setResolution(byte res, byte * addr)
{
    if (!my_ow->reset())
        return;
   
    if(addr)
        my_ow->select(addr);
    else
        my_ow->skip();   

    // Write scratchpad
    my_ow->write(0x4E);
    // Default TH/TL
    my_ow->write(0x4B);
    my_ow->write(0x46);
    // Control reg
    my_ow->write((res << 5) | 0x1F);

    if (!my_ow->reset())
        return;

    if(addr)
        my_ow->select(addr);
    else
        my_ow->skip();   
    // Copy scratchpad
    my_ow->write(0x48);
    delay(10);
    if (!my_ow->reset())
        return;

    

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

    #if DEBUG_TEMP_CONV
    Serial0.print( " Current delay:");
    Serial0.print(current_delay);
    Serial0.print( " Current resolution:");
    Serial0.print(current_resolution);
    #endif

    delay(int(current_delay*10)); 

    if(!my_ow->reset())
    	return temp;

    if(addr)
    	my_ow->select(addr);
    else
    	my_ow->skip();
        
    my_ow->write(0xBE);         // Read Scratchpad

    #if DEBUG_TEMP_CONV
    Serial0.print( " RAW:{ ");
    #endif

    for ( i = 0; i < 9; i++) 
    {           
      // we need 9 bytes
      dallas_data[i] = my_ow->read();
      #if DEBUG_TEMP_CONV
      Serial0.print(dallas_data[i], HEX);
      Serial0.print(" ");
      #endif
    }
    #if DEBUG_TEMP_CONV
    Serial0.println("}");
    #endif

    temp = dallas_data[1]; // For all other bits we have 1 when temperature < 0
    temp <<= 8;
    temp |= dallas_data[0];

    // extract current resolution from control register
    current_resolution = (dallas_data[4] >> 5) & 0x03;
    // current_delay ~ 10*2^resolution
    current_delay = 10;
    current_delay <<= current_resolution;
    // fill low bits with zeros
    // using current resolution
    temp &= ~((1 << (3 - current_resolution))-1);

    // We use fixed point calculations here
    // so the following sequence
    // temp /= 0.16 
    // will be
    // 16/100 => 4/25 => temp *= 25; tem/=4;
    temp *= 25;
    temp >>= 2;
    

    return temp;
}

float DS18B20Sensor::getTemperature(byte * addr)
{
    return getTempC100(addr) / 100.0;
       
}