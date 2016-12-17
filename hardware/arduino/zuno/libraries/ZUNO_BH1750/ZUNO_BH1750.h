/*************************************************** 
  This is a library for the BH1750(FVI) light sensor
  This sensor use I2C to communicate, 2 pins are required to  
  interface
  Datasheet:
  http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1750fvi-e.pdf
  Based on BH1750 lib written by Christopher Laws, March, 2013.
	Adapted to ZUNO
  ****************************************************/

#ifndef BH1750_h
#define BH1750_h

#include "Arduino.h"

// BH1750 address on the I2C bus, depending on the level of ADDR pin
#define BH1750_I2CADDR_L	0x23
#define BH1750_I2CADDR_H	0x5C
// define low address as default
#define BH1750_I2CADDR 		BH1750_I2CADDR_L

// No active state
#define BH1750_POWER_DOWN 					0x00

// Wating for measurment command
#define BH1750_POWER_ON 					0x01

// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_RESET 						0x07

// Start measurement at 1 lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE  	0x10

// Start measurement at 0.5 lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE_2	0x11

// Start measurement at 4 lx resolution. Measurement time is approx 16ms.
#define BH1750_CONTINUOUS_LOW_RES_MODE  	0x13

// Start measurement at 1 lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE  		0x20

// Start measurement at 0.5 lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE_2  	0x21

// Start measurement at 1 lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_LOW_RES_MODE  		0x23

// max measurement times in ms
#define BH1750_LOW_RES_MODE_MAXTIME			24
#define BH1750_HIGH_RES_MODE_MAXTIME		180
#define BH1750_HIGH_RES_MODE_2_MAXTIME	180


class BH1750 {
 public:
  BH1750();
  void 		begin(uint8_t mode = BH1750_CONTINUOUS_HIGH_RES_MODE, uint8_t addr = BH1750_I2CADDR_L);
  void 		configure(uint8_t mode = BH1750_CONTINUOUS_HIGH_RES_MODE, uint8_t addr = BH1750_I2CADDR_L);
  void		powerOn(uint8_t addr = BH1750_I2CADDR_L);
	void		powerDown(uint8_t addr = BH1750_I2CADDR_L);
	void		reset(uint8_t addr = BH1750_I2CADDR_L);
  uint16_t 	readLightLevel(uint8_t addr = BH1750_I2CADDR_L);
  
 private:
  uint8_t test; // dummy to get rid of error during compilation if no private part exist
};

#endif