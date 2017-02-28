#include <ZUNO_LPS331.h>
#include <Wire.h>

// Defines ///////////////////////////////////////////////////////////

// The Arduino two-WIRE_IMU interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define LPS331AP_ADDRESS_SA0_LOW  0b1011100
#define LPS331AP_ADDRESS_SA0_HIGH 0b1011101

#define WIRE_IMU Wire

// Constructors //////////////////////////////////////////////////////

LPS331::LPS331(void)
{
  // Pololu board pulls SA0 high, so default assumption is that it is
  // high
  address = LPS331AP_ADDRESS_SA0_LOW;
}

// Public Methods ////////////////////////////////////////////////////

// sets or detects slave address; returns bool indicating success
void LPS331::begin()
{
  WIRE_IMU.begin();
  writeReg(LPS331_CTRL_REG1, 0b11100000);
  delay(100);
}

// writes register
void LPS331::writeReg(byte reg, byte value)
{
  WIRE_IMU.beginTransmission(address);
  WIRE_IMU.write(reg);
  WIRE_IMU.write(value);
  WIRE_IMU.endTransmission();
}

// reads register
byte LPS331::readReg(byte reg)
{
  byte value;

  WIRE_IMU.beginTransmission(address);
  WIRE_IMU.write(reg);
  WIRE_IMU.endTransmission(false); // restart
  WIRE_IMU.requestFrom(address, (byte)1);
  value = WIRE_IMU.read();
  WIRE_IMU.endTransmission();

  return value;
}

// read pressure in pascals
float LPS331::readPressurePascals(void)
{
  return (float)readPressureRaw() / 40.96;
}

// reads pressure in millibars (mbar)/hectopascals (hPa)
float LPS331::readPressureMillibars(void)
{
  return (float)readPressureRaw() / 4096;
}

// reads pressure in inches of mercury (inHg)
float LPS331::readPressureInchesHg(void)
{
  return (float)readPressureRaw() / 138706.5;
}

// reads pressure in millimeters of mercury (mmHg)
float LPS331::readPressureMillimetersHg(void)
{
  // 1 mbar * 0,75006168270417 = 1 mmHg
  return (float)(readPressureRaw()) * 0.75006375541921 / 4096.0;
}

// reads pressure and returns raw 24-bit sensor output
int32_t LPS331::readPressureRaw(void)
{
  WIRE_IMU.beginTransmission(address);
  // assert MSB to enable register address auto-increment
  WIRE_IMU.write(LPS331_PRESS_OUT_XL | (1 << 7));
  WIRE_IMU.endTransmission();
  WIRE_IMU.requestFrom(address, (byte)3);

  while (WIRE_IMU.available() < 3);

  uint8_t pxl = WIRE_IMU.read();
  uint8_t pl = WIRE_IMU.read();
  uint8_t ph = WIRE_IMU.read();

  // combine bytes
  return (int32_t)(int8_t)ph << 16 | (uint16_t)pl << 8 | pxl;
}

// reads temperature in degrees K
float LPS331::readTemperatureK(){
  return readTemperatureC() + LPS331_CELSIUS_TO_KELVIN_OFFSET;
}

// reads temperature in degrees C
float LPS331::readTemperatureC(void)
{
  return 42.5 + (float)readTemperatureRaw() / 480;
}

// reads temperature in degrees F
float LPS331::readTemperatureF(void)
{
  return 108.5 + (float)readTemperatureRaw() / 480 * 1.8;
}

// reads temperature and returns raw 16-bit sensor output
int16_t LPS331::readTemperatureRaw(void)
{
  WIRE_IMU.beginTransmission(address);
  // assert MSB to enable register address auto-increment
  WIRE_IMU.write(LPS331_TEMP_OUT_L | (1 << 7));
  WIRE_IMU.endTransmission();
  WIRE_IMU.requestFrom(address, (byte)2);

  while (WIRE_IMU.available() < 2);

  uint8_t tl = WIRE_IMU.read();
  uint8_t th = WIRE_IMU.read();

  // combine bytes
  return (int16_t)(th << 8 | tl);
}


// Private Methods ///////////////////////////////////////////////////

bool LPS331::autoDetectAddress(void)
{
  // try each possible address and stop if reading WHO_AM_I returns the expected response
  address = LPS331AP_ADDRESS_SA0_LOW;
  if (testWhoAmI()) return true;
  address = LPS331AP_ADDRESS_SA0_HIGH;
  if (testWhoAmI()) return true;

  return false;
}

bool LPS331::testWhoAmI(void)
{
  return (readReg(LPS331_WHO_AM_I) == 0xBB);
}