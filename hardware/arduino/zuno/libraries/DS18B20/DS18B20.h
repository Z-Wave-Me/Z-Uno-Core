#ifndef DALLAS_DS_18B20LIB
#define DALLAS_DS_18B20LIB
#include "Arduino.h"
#include "OneWire.h"

#define BAD_TEMP -32767

class DS18B20Sensor
{
	public:
		DS18B20Sensor(OneWire * ow);

		// Определяет адрес сенсора, 
		// но только если он один на шине
		byte scanAloneSensor(byte * rom);
		// Считывает температуру, если первый параметр не задан 
		// считает что только одноустройство на шине
		// возвращает температуру в 10-х градуса Цельсия 
		int getTempC10(byte * addr = NULL);


	private:
		OneWire * my_ow;			
};

#endif // DALLAS_DS_18B20LIB
