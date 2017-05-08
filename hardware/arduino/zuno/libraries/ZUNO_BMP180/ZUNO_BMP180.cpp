/*************************************************** 
  This is a library for the Adafruit BMP085/BMP180 Barometric Pressure + Temp sensor

  Designed specifically to work with the Adafruit BMP085 or BMP180 Breakout 
  ----> http://www.adafruit.com/products/391
  ----> http://www.adafruit.com/products/1603

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "ZUNO_BMP180.h"
#include "Wire.h"


#define BMP180_DEBUG 0


#define DEBUG_SERIAL Serial1


#define _delay_ms(d) delay(d)

ZUNO_BMP180::ZUNO_BMP180() {
}


boolean ZUNO_BMP180::begin(uint8_t mode) {
  if (mode > BMP085_ULTRAHIGHRES) 
    mode = BMP085_ULTRAHIGHRES;
  oversampling = mode;

  Wire.begin();

  if (read8(0xD0) != 0x55) return false;

  /* read calibration data */
  ac1 = read16(BMP085_CAL_AC1);
  ac2 = read16(BMP085_CAL_AC2);
  ac3 = read16(BMP085_CAL_AC3);
  ac4 = read16(BMP085_CAL_AC4);
  ac5 = read16(BMP085_CAL_AC5);
  ac6 = read16(BMP085_CAL_AC6);

  b1 = read16(BMP085_CAL_B1);
  b2 = read16(BMP085_CAL_B2);

  mb = read16(BMP085_CAL_MB);
  mc = read16(BMP085_CAL_MC);
  md = read16(BMP085_CAL_MD);

}
void     ZUNO_BMP180::dumpInternal()
{
  DEBUG_SERIAL.println("---  BMP180 parameters ---");
  DEBUG_SERIAL.print("ac1 = "); DEBUG_SERIAL.print(ac1, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac1, HEX);
  DEBUG_SERIAL.print("ac2 = "); DEBUG_SERIAL.print(ac2, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac2, HEX);
  DEBUG_SERIAL.print("ac3 = "); DEBUG_SERIAL.print(ac3, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac3, HEX);
  DEBUG_SERIAL.print("ac4 = "); DEBUG_SERIAL.print((unsigned int)ac4, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac4, HEX);
  DEBUG_SERIAL.print("ac5 = "); DEBUG_SERIAL.print((unsigned int)ac5, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac5, HEX);
  DEBUG_SERIAL.print("ac6 = "); DEBUG_SERIAL.print((unsigned int)ac6, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(ac6, HEX);

  DEBUG_SERIAL.print("b1 = "); DEBUG_SERIAL.print(b1, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(b1, HEX);
  DEBUG_SERIAL.print("b2 = "); DEBUG_SERIAL.print(b2, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(b2, HEX);

  DEBUG_SERIAL.print("mb = "); DEBUG_SERIAL.print(mb, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(mb, HEX);
  
  DEBUG_SERIAL.print("mc = "); DEBUG_SERIAL.print(mc, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(mc, HEX);
  DEBUG_SERIAL.print("md = "); DEBUG_SERIAL.print(md, DEC); DEBUG_SERIAL.print(" HEX:"); DEBUG_SERIAL.println(md, HEX);
  DEBUG_SERIAL.println("--- ****************** ---");
  
}
uint16_t ZUNO_BMP180::readRawTemperature(void) {
  write8(BMP085_CONTROL, BMP085_READTEMPCMD);
  _delay_ms(5);
  return read16(BMP085_TEMPDATA);
}

uint32_t ZUNO_BMP180::readRawPressure(void) {
  uint32_t raw;

  write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (oversampling << 6));

  if (oversampling == BMP085_ULTRALOWPOWER) 
    _delay_ms(5);
  else if (oversampling == BMP085_STANDARD) 
    _delay_ms(8);
  else if (oversampling == BMP085_HIGHRES) 
    _delay_ms(14);
  else 
    _delay_ms(26);

  raw = read16(BMP085_PRESSUREDATA);

  raw <<= 8;
  raw |= read8(BMP085_PRESSUREDATA+2);
  raw >>= (8 - oversampling);

 
  return raw;
}


int32_t ZUNO_BMP180::readPressure(void) {

  int16_t UT = readRawTemperature();
  long    UP = readRawPressure();


  long X1 = UT - ac6;
  long X2 = mc;

  X1 *= ac5;
  X1 >>= 15;
  X2 <<= 11;
  X2 /= (X1 + md);
  //1) => B5 => B6 
  long B6 = (X1 + X2) - 4000;
  #if BMP180_DEBUG
  DEBUG_SERIAL.print("B6 = "); DEBUG_SERIAL.println(B6, DEC); ;
  #endif

  //2) X1
  // X1 = (B2*(B6*B6)/2^12)/2^11
  X2 = X1 = B6;
  X1 *= B6;
  X1 >>= 12;
  uint16_t koef1 = X1;
  X1 *= b2;
  X1 >>= 11;
  //3) 
  // X2 = ac2*b6/2^11
  X2 *= ac2;
  X2 >>= 11;
  // 4
  // B3 = ((4*ac1 + X1 + X2)*2^(oversampling) + 2) / 4
  long B3 = ac1;
  B3 <<= 2;
  B3 += X1 + X2;
  B3 <<= oversampling;
  B3 >>= 2;
  #if BMP180_DEBUG
  DEBUG_SERIAL.print("B3 = "); DEBUG_SERIAL.println(B3, DEC); ;
  #endif

  // 5)
  // X1 = AC3*B6/2^13
  X1 = ac3;
  X1 *= B6;
  X1 >>= 13;
  // 6)
  // X2 = (b1*b6*b6/2^12)/2^16
  X2 = b1;
  X2 *= koef1;
  X2 >>= 16;
  // 
  X1 += X2 + 2;
  X1 >>= 2;

  // B4....
  unsigned long B4 = X1;
  B4 += 32768;
  B4 *= ac4;
  B4 >>= 15;
  #if BMP180_DEBUG
  DEBUG_SERIAL.print("B4 = "); DEBUG_SERIAL.println(B4, DEC);
  #endif



  // B7
  unsigned long B7 = UP-B3; // B7
  B7 *= (50000 >> oversampling); 

  #if BMP180_DEBUG
  DEBUG_SERIAL.print("B7 = "); DEBUG_SERIAL.println(B7, DEC);
  #endif


  long p = (B7 & 0x80000000) ? (B7/B4) << 1 : (B7 << 1) / B4 ;
  
  #if BMP180_DEBUG
  DEBUG_SERIAL.print("p = "); DEBUG_SERIAL.println(p, DEC);
  #endif


  X1 = p >> 8;
  X1 *= X1;
  X1 *= 3038;
  X1 >>= 16;
  X2 = p;
  X2 *= -7357;
  X2 >>= 16;
  X1 += X2 + 3791;
  X1 >>= 4;

  p += X1; 

  return p;
}


int16_t ZUNO_BMP180::readTemperatureC10(void) {

 
  int16_t UT = readRawTemperature();
  //Serial.print("UT=");
  //Serial.println(UT);
  long X1 = UT - ac6;
  long X2 = mc;

  X1 *= ac5;
  X1 >>= 15;
  X2 <<= 11;
  X2 /= (X1 + md);
  X1 += X2 + 8;
  X1 >>= 4;

  return  (int16_t) X1;
}

float  ZUNO_BMP180::readTemperature(void)
{
    return readTemperatureC10() / 10.0;
}
float ZUNO_BMP180::readAltitude(float sealevelPressure) {
  float altitude;

  float pressure = readPressure();

  altitude = 44330 * (1.0 - pow(pressure /sealevelPressure,0.1903));

  return altitude;
}


/*********************************************************************/

uint8_t ZUNO_BMP180::read8(uint8_t a) {
  uint8_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  //Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  //Wire.endTransmission(); // end transmission

  return ret;
}

uint16_t ZUNO_BMP180::read16(uint8_t a) {
  uint16_t ret;

  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  //Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret <<= 8;
  ret |= Wire.read(); // receive DATA
  //Wire.endTransmission(); // end transmission

  return ret;
}

void ZUNO_BMP180::write8(uint8_t a, uint8_t d) {
  Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.write(d);  // write data
  Wire.endTransmission(); // end transmission
}
