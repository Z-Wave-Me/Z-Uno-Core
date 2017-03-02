/*************************************************** 
  This is a library for the BH1750(FVI) light sensor
  This sensor use I2C to communicate, 2 pins are required to  
  interface
  Datasheet:
  http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1750fvi-e.pdf
  Based on BH1750 lib written by Christopher Laws, March, 2013.
	Adapted to ZUNO
  ****************************************************/

#include "ZUNO_BH1750.h"
#include "Wire.h"

#define BH1750_DEBUG 0

BH1750::BH1750() {
	
}

void BH1750::begin(uint8_t mode, uint8_t addr) {

  Wire.begin();
  // initialization sensor
  configure(mode, addr);
}

void BH1750::configure(uint8_t mode, uint8_t addr) {

	Wire.beginTransmission(addr);
	switch (mode) {
			case BH1750_CONTINUOUS_HIGH_RES_MODE:
			case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
			case BH1750_CONTINUOUS_LOW_RES_MODE:
			case BH1750_ONE_TIME_HIGH_RES_MODE:
			case BH1750_ONE_TIME_HIGH_RES_MODE_2:
			case BH1750_ONE_TIME_LOW_RES_MODE:
					// apply a valid mode change
					Wire.write(mode);
		delay(10);
					break;
			default:
					// Invalid measurement mode
					#if BH1750_DEBUG == 1
					Serial.println("Invalid measurement mode");
					#endif
					break;
	}
	Wire.endTransmission();
}

void BH1750::powerOn(uint8_t addr) {
	Wire.beginTransmission(addr);
  Wire.write(BH1750_POWER_ON);
  Wire.endTransmission();
}
void BH1750::powerDown(uint8_t addr) {
	Wire.beginTransmission(addr);
  Wire.write(BH1750_POWER_DOWN);
  Wire.endTransmission();
}
void BH1750::reset(uint8_t addr) {
	Wire.beginTransmission(addr);
  Wire.write(BH1750_RESET);
  Wire.endTransmission();
}

uint16_t BH1750::readLightLevel(uint8_t addr) {

  uint16_t level;

  Wire.requestFrom(addr, 2);

  level = Wire.read();
  level <<= 8;
  level |= Wire.read();

#if BH1750_DEBUG == 1
  Serial.print("Raw light level: ");
  Serial.println(level);
#endif

  //level = level/1.2; // convert to lux

  level *= 6;
  level /= 5;

#if BH1750_DEBUG == 1
  Serial.print("Light level: ");
  Serial.println(level);
#endif
  return level;
}