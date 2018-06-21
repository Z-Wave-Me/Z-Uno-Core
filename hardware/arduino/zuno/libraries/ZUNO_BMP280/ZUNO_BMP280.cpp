/*************************************************** 
  This is a library for the Bosch BMP280/BME280 Barometric Pressure + Temp sensor + Humidity sensor

  Designed specifically to work with the Bosch BMP280 or BME280 sensors 
  Writen by Z-Wave>ME team for Z-Uno project.
  Used code from:
  1) Written by Limor Fried/Ladyada for Adafruit Industries.  
      https://github.com/adafruit/Adafruit_BME280_Library
      https://github.com/adafruit/Adafruit_BMP280_Library

  2) Bosch datasheets
    https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf
    https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf

  Slightly adoptated for fix-point i8051 math and Z-Uno stack behaviour.
 ****************************************************/

#include "ZUNO_BMP280.h"
#include "Wire.h"


#define BMP180_DEBUG 0


#define DEBUG_SERIAL Serial0


#define _delay_ms(d) delay(d)

#define BME280_S32_t int32_t
#define BME280_U32_t uint32_t

extern byte g_myi2c_addr;

void aux_swap16LE(XBYTE * value);
uint8_t aux_read8(uint8_t a);
void aux_read16(uint8_t a, XBYTE * value);
void aux_read20(uint8_t a, XBYTE * value);
void aux_write8(uint8_t a, uint8_t d);

ZUNO_BMP280::ZUNO_BMP280() {
}


boolean ZUNO_BMP280::begin(uint8_t addr) {

  byte tmp;
  Wire.begin();

  this->addr = addr;
  g_myi2c_addr = addr;

  chip_id =  aux_read8(BME280_REGISTER_CHIPID);
  if ((chip_id != CHIPID_BMP280) &&
      (chip_id != CHIPID_BME280)) 
    return false;
  aux_write8(BME280_REGISTER_SOFTRESET, 0xB6);
  // wait for chip to wake up.
  delay(300);
  // waiting for calibration
  while(aux_read8(BME280_REGISTER_STATUS) & 0x01)
    delay(100);

  aux_read16(BME280_REGISTER_DIG_T1, (XBYTE*)&dig_T1);
  aux_read16(BME280_REGISTER_DIG_T2, (XBYTE*)&dig_T2);
  aux_read16(BME280_REGISTER_DIG_T3, (XBYTE*)&dig_T3);
  aux_read16(BME280_REGISTER_DIG_P1, (XBYTE*)&dig_P1);
  aux_read16(BME280_REGISTER_DIG_P2, (XBYTE*)&dig_P2);
  aux_read16(BME280_REGISTER_DIG_P3, (XBYTE*)&dig_P3);
  aux_read16(BME280_REGISTER_DIG_P4, (XBYTE*)&dig_P4);
  aux_read16(BME280_REGISTER_DIG_P5, (XBYTE*)&dig_P5);
  aux_read16(BME280_REGISTER_DIG_P6, (XBYTE*)&dig_P6);
  aux_read16(BME280_REGISTER_DIG_P7, (XBYTE*)&dig_P7);
  aux_read16(BME280_REGISTER_DIG_P8, (XBYTE*)&dig_P8);
  aux_read16(BME280_REGISTER_DIG_P9, (XBYTE*)&dig_P9);

  if(chip_id == CHIPID_BME280) {
    dig_H1 = aux_read8(BME280_REGISTER_DIG_H1);
    aux_read16(BME280_REGISTER_DIG_H2, (XBYTE*)&dig_H2);
    dig_H3 = aux_read8(BME280_REGISTER_DIG_H3);
    dig_H4 = (aux_read8(BME280_REGISTER_DIG_H4) << 4) | (aux_read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    dig_H5 = (aux_read8(BME280_REGISTER_DIG_H5+1) << 4) | (aux_read8(BME280_REGISTER_DIG_H5) >> 4);
    dig_H6 = aux_read8(BME280_REGISTER_DIG_H6);
  }
  // Defaults
  aux_write8(BME280_REGISTER_CONTROL, 0x24);
  delay(20);
  aux_write8(BME280_REGISTER_CONFIG, 0x05<<5);
  delay(20);
  aux_write8(BME280_REGISTER_CONTROLHUMID, 0x01);
  aux_write8(BME280_REGISTER_CONTROL, (0x7 << 5) | (7<<2) | 0x03);
  
  return true;
  

}

dword adc_V;
word adc_H;
// --------------------------------------------------------------
// auxilary variables for calculations
uint32_t p;
int32_t t_fine;
int32_t v3, v1, v2; 
// --------------------------------------------------------------



int32_t ZUNO_BMP280::readPressurePa(void) {
  
  readTemperatureC100(); // must be done first to get t_fine
  aux_read20(BME280_REGISTER_PRESSUREDATA, (XBYTE*)&adc_V);
  if (adc_V == 0x80000) // value in case pressure measurement was disabled
        return BAD_BMP280_32BVALUE;

  /*
  var1 = (((BME280_S32_t)t_fine)>>1)-(BME280_S32_t)64000;
  var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BME280_S32_t)dig_P6);
  */

  v1 = t_fine;
  v1 >>= 1;
  v1 -= 64000;
  v2 = v1;
  v2 >>= 2;
  v2 *= v2;
  v2 >>= 11;
  v2 *= dig_P6;
  /*
  var2 = var2 + ((var1*((BME280_S32_t)dig_P5))<<1);
  var2 = (var2>>2)+(((BME280_S32_t)dig_P4)<<16);
  */
  v3 = v1;
  v3 *= dig_P5;
  v3 <<= 1;
  v3 += v2;
  v3 >>= 2;
  v2 = dig_P4;
  v2 <<= 16;
  v2 += v3;
  /*
  var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BME280_S32_t)dig_P2) * var1)>>1))>>18; 
  */
  v3 = v1;
  v3 >>= 2;
  v3 *= v3;
  v3 >>= 13;
  v3 *= dig_P3;
  v1 *= dig_P2;
  v1 >>= 1;
  v1 += v3;
  v1 >>= 18;
  /*
  var1 = ((((32768+var1))*((BME280_S32_t)dig_P1))>>15);
  */
  v1 += 32768;
  v1 *= dig_P1;
  v1 >>= 15;
  if (v1 == 0)
    return 0; // avoid exception caused by division by zero 
  /*
  p = (((BME280_U32_t)(((BME280_S32_t)1048576)-adc_P)-(var2>>12)))*3125; 
  */
  p = 1048576;
  p -= adc_V;
  v2 >>= 12;
  p -= v2;
  p *= 3125;
  if (p < 0x80000000) {
      /*
      p = (p << 1) / ((BME280_U32_t)var1); 
      */
      p <<= 1;
      p /= v1;
  } else {
      /*
      p = (p / (BME280_U32_t)var1) * 2;
      */
      p /= v1;
      p <<= 1;
      
  }
  /*
  var1 = (((BME280_S32_t)dig_P9) * ((BME280_S32_t)(((p>>3) * (p>>3))>>13)))>>12; 
  */
  v1 = p;
  v1 >>= 3;
  v1 *= v1;
  v1 >>= 13;
  v1 *= dig_P9;
  v1 >>= 12;
  /*
  var2 = (((BME280_S32_t)(p>>2)) * ((BME280_S32_t)dig_P8))>>13;
  */
  v2 = p;
  v2 >>= 2;
  v2 *= dig_P8;
  v2 >>= 13;
  /*
  p = (BME280_U32_t)((BME280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
  */
  v2 += v1;
  v2 += dig_P7;
  v2 >>= 4;
  p += v2;
  return p;
  
}
int32_t ZUNO_BMP280::readPressureHgMM10(void){
  readPressurePa();
  p *= 1000;
  p /= 13333;
  return p;
}

int16_t ZUNO_BMP280::readTemperatureC100(void) {
  g_myi2c_addr = addr;
  aux_read20(BME280_REGISTER_TEMPDATA, (XBYTE*) &adc_V);
  if (adc_V == 0x80000) // value in case temp measurement was disabled
        return BAD_BMP280_16BVALUE;
 /*
 // Code from the Bosch datasheet
 // It's not good for 8051
 var1 = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
 var2 = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *((BME280_S32_t)dig_T3)) >> 14; 
 t_fine = var1 + var2; 
 Lets do it in RISC style :)
 */
 
 // ~var1, see^
 v1 = adc_V;
 v1 >>= 3;
 v2 = dig_T1;
 v2 <<= 1;
 v1 -= v2;
 v1 *= dig_T2;
 v1 >>= 11; 
 t_fine = v1;
 // ~var2, see^
 v1 = adc_V;
 v1 >>= 4;
 v1 -= dig_T1;
 v1 *= v1;
 v1 >>= 12;
 v1 *= dig_T3;
 v1 >>= 14;
 // t_fine = var1 + var2;
 t_fine += v1;
 // conver t_fine to fixed point temperature
 v1 = t_fine;
 v1 *= 5;
 v1 += 128;
 v1 >>= 8;
 return int16_t(v1);
}
int16_t ZUNO_BMP280::readHumidityH10(void) {
    if(chip_id != CHIPID_BME280)
      return 0;
    readTemperatureC100(); // must be done first to get t_fine
    aux_read16(BME280_REGISTER_HUMIDDATA, (XBYTE*)&adc_H);
    aux_swap16LE((XBYTE*)&adc_H);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return BAD_BMP280_16BVALUE;
    v1 = t_fine;
    v1 -= 76800; 
    /*
    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
                    (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));
    
    
    */
    v2 = v1;
    v2 *= dig_H6;
    v2 >>= 10;
    v3 = v2;
    // ((v_x1_u32r * ((int32_t)dig_H3)) >> 11)
    v2 = v1;
    v2 *= dig_H3;
    v2 >>= 11;
    // + ((int32_t)32768)
    v2 += 32768;
    v3 *= v2;
    v3 >>= 10;
    v3 += 2097152;
    v3 *= dig_H2;
    v3 += 8192;
    v3 >>= 14;
    v1 *= dig_H5; 
    v2 =  dig_H4;
    v2 <<= 20;
    v1 += v2;
    v2 = adc_H;
    v2 <<= 14;
    v1 = v2 - v1;
    v1 += 16384;
    v1 >>= 15;
    v3 *= v1;
    /*

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)dig_H1)) >> 4));
    */
    v1 =  v3;
    v1 >>= 15;
    v1 *= v1;
    v1 >>= 7;
    v1 *= dig_H1;
    v1 >>= 4;
    v3 -= v1;
    v3 = (v3 < 0) ? 0 : v3;
    v3 = (v3 > 419430400) ? 419430400 : v3;
    v3 >>= 12;
    v3 *= 10;
    v3 >>= 10;
    return  int16_t(v3);

}
float  ZUNO_BMP280::readTemperature(void)
{
    return readTemperatureC100() / 100.0;
}



/*********************************************************************/

byte g_myi2c_addr = BME280_ADDRESS;
void aux_swap16LE(XBYTE * value) {
  byte tmp = value[0];
  value[0] = value[1];
  value[1] = tmp;
}
uint8_t aux_read8(uint8_t a) {
  uint8_t ret;

  Wire.beginTransmission(g_myi2c_addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  Wire.requestFrom(g_myi2c_addr, 1);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  return ret;
}

void aux_read16(uint8_t a, XBYTE * value) {

  Wire.beginTransmission(g_myi2c_addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  
  Wire.requestFrom(g_myi2c_addr, 2);// send data n-bytes read
  // !!! SDCC specific
  value[0] = Wire.read(); // receive DATA
  value[1] = Wire.read(); // receive DATA

}

void aux_write8(uint8_t a, uint8_t d) {
  Wire.beginTransmission(g_myi2c_addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.write(d);  // write data
  Wire.endTransmission(); // end transmission
}
void aux_read20(uint8_t a, XBYTE * value) {
  // USING SDCC DWORD representaion
  // This code is not portable
  value[3] = 0;
  Wire.beginTransmission(g_myi2c_addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(); // end transmission
  Wire.requestFrom(g_myi2c_addr, 3);// send data n-bytes read
  value[2] = Wire.read(); 
  value[1] = Wire.read();
  value[0] = Wire.read();
  // make 20 bit value
  *((dword*)value) >>= 4;
}
