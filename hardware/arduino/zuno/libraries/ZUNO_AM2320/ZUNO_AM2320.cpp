#include "ZUNO_AM2320.h"

#define MY_ADDRESS 			0x5C
#define CMD_READ_REGS		0x03


byte g_am2320_data[8];
word g_crc;
word g_amcalc_crc;
byte g_am_cnt;


void createCRC16_AM2320();



void	AM2320::begin()
{
			Wire.begin();
			TIM_request = 0;
}


void	AM2320::read(bool force)
{
			byte result;
			err_code =  AM2320_OK;

			// don't check it very often
			if(((millis() - TIM_request) < 500) && (TIM_request != 0) && (!force))
			{
				return;
			}

			// Try to wake up the sensor...
			Wire.beginTransmission(MY_ADDRESS, true); // We have to send write command here	
			delayMicroseconds(80);					  // Wait at least 80 uS to wakeup sensor	
			result=Wire.endTransmission();						
			// Sending request to sensor
			Wire.beginTransmission(MY_ADDRESS);		// We have to read some registers from sensor 
			Wire.write(CMD_READ_REGS);				// Command		
			Wire.write(0x00);						// first reg address		
			Wire.write(0x04);						// number of regs to read			
			result=Wire.endTransmission();	

			// Sensor is not responing...
			if(result != 0)
			{
				err_code =  AM2320_ERROR_NOT_RESPONDING;
				return;
			}
			// We have to wait a little (see datasheet for more details)
			delayMicroseconds(1800);
			
			// We read 8 bytes, but we need 4 bytes of data + 2 of CRC16
			Wire.requestFrom(MY_ADDRESS,0x08);
			
			result = 0;
			while(Wire.available())
			{
				g_am2320_data[result++]	= Wire.read();
			
			}

			// Checking CRC16 
			createCRC16_AM2320();
			g_crc = g_am2320_data[7];
			g_crc <<= 8;
			g_crc |= g_am2320_data[6];

			if(g_amcalc_crc != g_crc)
			{
				err_code = AM2320_ERROR_CRC;
				return;
			}
			// Save last request time
			TIM_request=millis(); 
}
// returns temperature in 10 th of Celsius
int AM2320::getTemperatureC10(bool force)
{
	int res;
	read(force);
	if(err_code != 0)
		return 0;
	res = g_am2320_data[4];
	res <<= 8;
	res |= g_am2320_data[5]; 

	return res;
}
// returns humidity in 10 th of percent
int AM2320::getHumidityH10(bool force)
{
	int res;
	read(force);
	if(err_code != 0)
		return 0;
	res = g_am2320_data[2];
	res <<= 8;
	res |= g_am2320_data[3]; 

	return res;
}
byte * AM2320::rawData()
{
	return g_am2320_data;
}
// Returns temperature as float in Celsius
float AM2320::getTemperature(bool force)
{
	return getTemperatureC10(force)/10.0;
}
// Returns humidity as float in pecents
float AM2320::getHumidity(bool force)
{
	return getHumidityH10(force)/10.0;
}
 
void createCRC16_AM2320()
{
			g_amcalc_crc = 0xFFFF;									
			for(uint8_t i=0; i<6; i++)
			{								
				g_amcalc_crc ^= g_am2320_data[i];
														
				g_am_cnt = 8;
				while(g_am_cnt--)
				{
					if(g_amcalc_crc & 0x01)
					{
						g_amcalc_crc >>= 1;
						g_amcalc_crc ^= 0xA001;	
					}
					else
					{
						g_amcalc_crc >>= 1;
					}

				}
				
			}														
}