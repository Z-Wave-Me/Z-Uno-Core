/* DHT library

MIT license
written by Adafruit Industries

UPGRADED/Rewriten by Z-Wave>ME for project Z-Uno 2016

*/
#ifndef DHT_H
#define DHT_H

#include "Arduino.h"


// Define types of sensors.
#define DHT11   11
#define DHT22   22
#define DHT21   21
#define AM2301  21

#define BAD_DHT_VALUE 0xFFFF

enum
{
    DHT_RESULT_OK = 0,
    DHT_RESULT_PREVIOUS,
    DHT_RESULT_ERROR_NOSYNC,
    DHT_RESULT_ERROR_TIMEOUT,
    DHT_RESULT_ERROR_CRC

};


class DHT {
  public:
   DHT(s_pin pin, uint8_t type = DHT22);
   void begin(void);
   // returns temperature in 10 th of Celsius
   int readTemperatureC10(bool force=false);
   // returns humidity in 10 th of percent
   int readHumidityH10(bool force=false);

   // Returns temperature as float in Celsius
   float readTemperature(bool force=false);
   // Returns humidity as float in pecents
   float readHumidity(bool force=false);
   
   // Just read raw data from sensor

   // Returns result code of operation (see enum)
   byte read(bool force=false);

   // Copies raw data from the last "read" operation to user buffer
   void getRawData(byte * ptr){ byte i; for(i=0;i<5;i++) ptr[i] = data_ptr[i];};

 private:

  s_pin _pin;
  byte data_ptr[5];

  int16_t  humidity;
  int16_t  temperature;
  uint8_t  crc;
    
  uint8_t _type;
  uint32_t _lastreadtime, _maxcycles;
  bool _lastresult;


};


#endif
