// demo sketch for connecting I2C light sensor BH1750 to Z-Uno using ZUNO_BH1750 library

#include "ZUNO_BH1750.h"
#define DEBUG

BH1750 bh1750;

// BH1750 address on the bus: BH1750_I2CADDR_L = 0x23
#define BH1750_I2CADDR  BH1750_I2CADDR_L

word lightLux;

// set up channel
//ZUNO_SETUP_CHANNELS(
//      ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_LUMINANCE,
//                             SENSOR_MULTILEVEL_SCALE_LUX,  
//                             SENSOR_MULTILEVEL_SIZE_TWO_BYTES, 
//                             SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, 
//                             getterLight)
//);
// channel definition as one line to get error message reported on correct line (will be fixed in later version of ZUNO)
ZUNO_SETUP_CHANNELS( ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_LUMINANCE, SENSOR_MULTILEVEL_SCALE_LUX, SENSOR_MULTILEVEL_SIZE_TWO_BYTES, SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, getterLight));

void setup() {
  #ifdef DEBUG
      Serial.begin();
  #endif

  // Start continous measurement in HIGH_RES_MODE at 1 lx resolution
  // Measurement time is approx 120ms. max time 180ms defined as BH1750_HIGH_RES_MODE_MAXTIME
  bh1750.begin(BH1750_CONTINUOUS_HIGH_RES_MODE, BH1750_I2CADDR);

  #ifdef DEBUG
    Serial.println("start");
  #endif
}

void loop() {
  lightLux = bh1750.readLightLevel(BH1750_I2CADDR);
  
  #ifdef DEBUG
    Serial.print("light = ");
    Serial.println(lightLux);
  #endif
  zunoSendReport(1); 
  // send every 30 sec
  delay(30000);
}

word getterLight() {
  return lightLux;
}

