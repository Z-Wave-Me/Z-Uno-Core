/* DHT library

  MIT license
  written by Adafruit Industries

  UPGRADED/Rewriten by Z-Wave>ME for project Z-Uno 2016
*/

#include "ZUNO_DHT.h"

#define MIN_INTERVAL 2000

#define DEBUG_DHT 0

DHT::DHT(uint8_t pin, uint8_t type): _pin(pin) 
{
  _type = type;
                                                 // reading pulses from DHT sensor.
 
}

void DHT::begin(void) {
  // set up the pins!
  pinMode(_pin, INPUT_PULLUP);
  _lastreadtime = 0;
}

// returns temperature in 10 th of Celsius
int DHT::readTemperatureC10(bool force)
{

  if(read(force) > DHT_RESULT_PREVIOUS)
      return BAD_DHT_VALUE;

  return temperature;
  
}
   // returns humidity in 10 th of percent
int DHT::readHumidityH10(bool force)
{
   if(read(force) > DHT_RESULT_PREVIOUS)
      return BAD_DHT_VALUE;
    return humidity;   
}

   // Returns temperature in float in Celsius
float DHT::readTemperature(bool force)
{
    return readTemperatureC10(force) / 10.0;
}
   // Returns humidity in float
float DHT::readHumidity(bool force)
{
    return readHumidityH10(force) / 10.0;
}
   


// DBG
#if DEBUG_DHT
byte dbg_timings_s[80];
#endif


byte DHT::read(bool force) {

  byte time_i1, time_i2;
  word wi = 0;
  byte time_ref;
  //s_pin tp = 12;
  byte i = 0;
  byte ci = 0;
  byte cb =0;
  

  
  //pinMode(tp, OUTPUT);
  //digitalWrite(tp, 0);
  
  // Чтобы не опрашивать сенсор слишком часто
  uint32_t currenttime = millis();
  if (!force && ((currenttime - _lastreadtime) < 2000)) {
    return DHT_RESULT_PREVIOUS; // return last correct measurement
  }
  _lastreadtime = currenttime;
  

  // Посылаем стартовый импульс 
  // 0 на 1 мс и болльше
  //noInterrupts();
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW); // Send start signal
  switch(_type)
  {
    case DHT11:
          delay(13); // Датчик долго думает
          break;
    default:
          delayMicroseconds(1400); // Гораздо быстрее откликается
          break;
  }
  pinMode(_pin, INPUT_PULLUP);

  noInterrupts();
  time_i1 = 0; 
  while(digitalRead(_pin))//digitalRead(_pin))
  {
    wi++;
    if(!wi) 
    {
      interrupts();  
      return DHT_RESULT_ERROR_NOSYNC;
    }
    NOPS(8);
  }
  time_i1 = 0;
  while(!digitalRead(_pin))
  {
    time_i1++;
    if(!time_i1)
      break;
  }
  time_i2 = 0;
  while(digitalRead(_pin))
  {
    time_i2++;
    if(!time_i2)
      break;
  }
 
  if(time_i1 < 10 || time_i2 < 10)
  {

      interrupts();
      return DHT_RESULT_ERROR_NOSYNC;  
  }

  // Вычисляем опорное время для "1"
  // == Половина среднего времени отклика датчика
  // Вычисляется здесь т.к. 
  // время стартового интервала будет плавать в цикле чтения битов
  // особенно это заметно на каждом 8-ом бите 
  time_ref = time_i1 + time_i2;
  time_ref >>= 2; 

  while(i<80)
  {
        
      switch(i&0x01)
      {
         case 0:
            time_i1 =  0;
            while(!digitalRead(_pin)) time_i1++;
            #if DEBUG_DHT
            dbg_timings_s[i] = time_i1;
            #endif
            break;
          case 1:
            time_i2 = 0; 
            while(digitalRead(_pin)) time_i2++;
            #if DEBUG_DHT
            dbg_timings_s[i] = time_i2;
            #endif
            cb <<= 1;
            cb |=  (time_i2 > time_ref);
            break;    
      }

      i++;
      if((i & 0x0F)==0)
      {
        data_ptr[ci++] = cb;
        cb = 0;
      }
  }

  interrupts();
  

  
  #if DEBUG_DHT
  Serial.print("TIMINGS \n");
  for(i=0;i<80;i++)
  {
      Serial.print("EDGE #");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(dbg_timings_s[i]);
    
  }
  Serial.print("RAW ");
  for(i=0;i<5;i++)
  {
      Serial.print((uint16_t)data_ptr[i], HEX);
      Serial.print(" ");  
  }
  Serial.println(" ");
  #endif

  byte sum = data_ptr[0];
  sum += data_ptr[1];
  sum += data_ptr[2];
  sum += data_ptr[3];
  
   
  if ( sum != data_ptr[4]) 
  {
      #if DEBUG_DHT
      Serial.print(" Calculated sum: "); 
      Serial.print(sum, HEX);
      Serial.println(" "); 
      #endif
      return DHT_RESULT_ERROR_CRC;
  }

  
  

  if(_type == DHT11)
  {
      humidity =  data_ptr[0];
      temperature =  data_ptr[2];
      humidity *= 10;
      temperature *= 10;   
  }
  else
  {
       
      humidity = data_ptr[0];
      humidity <<= 8;
      humidity += data_ptr[1];

      temperature = data_ptr[2] & ~(0x80);
      temperature <<= 8;
      temperature += data_ptr[3];       
      if ( data_ptr[2] & 0x80) 
          temperature -= temperature;       

        
  }

  return DHT_RESULT_OK;
}

