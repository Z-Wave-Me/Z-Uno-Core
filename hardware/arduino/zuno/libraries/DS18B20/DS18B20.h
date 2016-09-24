#ifndef DALLAS_DS_18B20LIB
#define DALLAS_DS_18B20LIB
#include "Arduino.h"
#include "OneWire.h"

#define BAD_TEMP -32767

class DS18B20Sensor
{
	public:
		DS18B20Sensor(OneWire * ow);

		// Search for sensors. Copies found sensor's address (ROM) to buffer
		// Returns:
		//   1 if we have a valid sensor connected
		//   0 otherwise    
		byte scanAloneSensor(byte * rom);
		
		// Return temperarure in cents of Celsius
		// sonsume less memory of sketch than getTemperature()
		int getTempC100(byte * addr = NULL);
		// Returns temperature as float in Celsius
		float getTemperature(byte * addr = NULL);


	private:
		OneWire * my_ow;			
};

#endif // DALLAS_DS_18B20LIB
