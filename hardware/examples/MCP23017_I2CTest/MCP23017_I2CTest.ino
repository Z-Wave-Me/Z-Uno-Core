 /*
  *     Тестирование шины I2C с помощью распостраненного порт-экстендера MCP23017
  *     Есть 2-ва идентичных варианта кода: На C++ и без его использования
  *     Пины порт-экстендера:
  *     15,16,17 -> GND (Адрес == 0)
  *     18       -> 3v3 (Reset подтянут)
  *     10       -> GND (Питание)
  *     9        -> 3v3 (Питание)
  *     12       -> ZUNO(9) SCL
  *     13       -> ZUNO(10) SDA
  */
#include "Wire.h"

 
void setup()
{
 
 
 // CXX version
 Wire.begin(); // wake up I2C bus
// set I/O pins to outputs
 Wire.beginTransmission(0x20);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 
 // Raw C
 /*
 zunoI2CInit();
 zunoI2CBegin();
 zunoI2CWrite(0x40);
 zunoI2CWrite(0x00);
 zunoI2CWrite(0x00);
 zunoI2CEnd();
*/
 Serial.begin();
 

}
 
void loop()
{

  Serial.print("Loop millis: ");
  Serial.println(millis());
  
  
  
  // CXX version
  Wire.beginTransmission(0x20);
  Wire.write(0x12);      // address bank A
  Wire.write((byte)0xAA);  // value to send - all HIGH
  Wire.endTransmission();
  delay(500);
  Wire.beginTransmission(0x20);
  Wire.write(0x12);      // address bank A
  Wire.write((byte)0x55);  // value to send - all HIGH
  Wire.endTransmission();
  delay(500);
  
  
  
  // RawC
  /*
  zunoI2CBegin();
  zunoI2CWrite(0x40);
  zunoI2CWrite(0x12);
  zunoI2CWrite(0xAA);
  zunoI2CEnd();
  delay(500);
  zunoI2CBegin();
  zunoI2CWrite(0x40);
  zunoI2CWrite(0x12);
  zunoI2CWrite(0x55);
  zunoI2CEnd();
  delay(500);
  */



}