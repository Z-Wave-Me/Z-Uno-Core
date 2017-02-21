
#ifndef ZUNO_AM2320_h
#define ZUNO_AM2320_h

#include <Arduino.h>
#include <Wire.h>
enum
{
	AM2320_OK,
	AM2320_ERROR_NOT_RESPONDING,
	AM2320_ERROR_CRC
};

class AM2320
{
	public:	
		AM2320(){};	
		void		begin();						
		
		 // returns temperature in 10 th of Celsius
   		int getTemperatureC10(bool force=false);
   		// returns humidity in 10 th of percent
   		int getHumidityH10(bool force=false);

   		// Returns temperature as float in Celsius
   		float getTemperature(bool force=false);
   		// Returns humidity as float in pecents
   		float getHumidity(bool force=false);
   

   		byte getLastError(){return err_code;};
   		byte * rawData();
	
	private:
		void		read(bool force);						
		
		uint32_t	TIM_request;	
		byte        err_code;
};

#endif // ZUNO_AM2320_h