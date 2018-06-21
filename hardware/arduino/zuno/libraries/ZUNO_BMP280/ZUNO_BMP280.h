/*************************************************** 
  This is a library for the Bosch BMP280/BME280 Barometric Pressure + Temp sensor + Humidity sensor

  Designed specifically to work with the Bosch BMP280 or BME280 sensors 
  Writen by Z-Wave>ME team for Z-Uno project.
  Used code from:
  1) Written by Limor Fried/Ladyada for Adafruit Industries.  
  2) From Bosch datasheet
  Slightly adoptated for fix-point math ans Z-Uno stack behaviour.
 ****************************************************/

#ifndef ZME_BMP280_H
#define ZNE_BMP280_H

#include "Arduino.h"

#define BMP085_DEBUG 0
#define BAD_BMP280_16BVALUE 0xFFFF
#define BAD_BMP280_32BVALUE 0xFFFFFFFF
/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define BME280_ADDRESS                (0x77)
/*=========================================================================*/

enum {
    CHIPID_BMP280 = 0x58,
    CHIPID_BME280 = 0x60
      
};
/*=========================================================================
  REGISTERS
 -----------------------------------------------------------------------*/
enum
    {
        BME280_REGISTER_DIG_T1              = 0x88,
        BME280_REGISTER_DIG_T2              = 0x8A,
        BME280_REGISTER_DIG_T3              = 0x8C,

        BME280_REGISTER_DIG_P1              = 0x8E,
        BME280_REGISTER_DIG_P2              = 0x90,
        BME280_REGISTER_DIG_P3              = 0x92,
        BME280_REGISTER_DIG_P4              = 0x94,
        BME280_REGISTER_DIG_P5              = 0x96,
        BME280_REGISTER_DIG_P6              = 0x98,
        BME280_REGISTER_DIG_P7              = 0x9A,
        BME280_REGISTER_DIG_P8              = 0x9C,
        BME280_REGISTER_DIG_P9              = 0x9E,

        BME280_REGISTER_DIG_H1              = 0xA1,
        BME280_REGISTER_DIG_H2              = 0xE1,
        BME280_REGISTER_DIG_H3              = 0xE3,
        BME280_REGISTER_DIG_H4              = 0xE4,
        BME280_REGISTER_DIG_H5              = 0xE5,
        BME280_REGISTER_DIG_H6              = 0xE7,

        BME280_REGISTER_CHIPID             = 0xD0,
        BME280_REGISTER_VERSION            = 0xD1,
        BME280_REGISTER_SOFTRESET          = 0xE0,

        BME280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

        BME280_REGISTER_CONTROLHUMID       = 0xF2,
        BME280_REGISTER_STATUS             = 0XF3,
        BME280_REGISTER_CONTROL            = 0xF4,
        BME280_REGISTER_CONFIG             = 0xF5,
        BME280_REGISTER_PRESSUREDATA       = 0xF7,
        BME280_REGISTER_TEMPDATA           = 0xFA,
        BME280_REGISTER_HUMIDDATA          = 0xFD
    };

/*=========================================================================*/

enum sensor_sampling {
            SAMPLING_NONE = 0b000,
            SAMPLING_X1   = 0b001,
            SAMPLING_X2   = 0b010,
            SAMPLING_X4   = 0b011,
            SAMPLING_X8   = 0b100,
            SAMPLING_X16  = 0b101
        };
enum sensor_mode {
            MODE_SLEEP  = 0b00,
            MODE_FORCED = 0b01,
            MODE_NORMAL = 0b11
          };
enum sensor_filter {
            FILTER_OFF = 0b000,
            FILTER_X2  = 0b001,
            FILTER_X4  = 0b010,
            FILTER_X8  = 0b011,
            FILTER_X16 = 0b100
        };
enum standby_duration {
            STANDBY_MS_0_5  = 0b000,
            STANDBY_MS_10   = 0b110,
            STANDBY_MS_20   = 0b111,
            STANDBY_MS_62_5 = 0b001,
            STANDBY_MS_125  = 0b010,
            STANDBY_MS_250  = 0b011,
            STANDBY_MS_500  = 0b100,
            STANDBY_MS_1000 = 0b101
        };

class ZUNO_BMP280 {
 public:
  ZUNO_BMP280();
  boolean begin(uint8_t addr=BME280_ADDRESS); 
  int16_t readTemperatureC100(void);
  float   readTemperature(void);
  int32_t readPressurePa(void);
  int32_t readPressureHgMM10(void);
  int16_t readHumidityH10(void);
  float   readHumidity(void);
 private:
  uint8_t addr;

  uint8_t oversampling;

  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
  int8_t   chip_id;
};


#endif //  ZME_BMP280_H
