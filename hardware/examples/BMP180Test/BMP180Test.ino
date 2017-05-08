/*************************************************** 
  This is an example for the BMP180 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  

  2016 Adoptated for Z-Uno project by Z-Wave>ME

  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on Z-Uno thats Digital 9
// Connect SDA to i2c data - on Z-Uno thats Digital 10
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

#include <Wire.h>
#include <ZUNO_BMP180.h>



ZUNO_BMP180 bmp;
  
void setup() {
  Serial.begin();
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP180 sensor, check wiring!");
  }
}
  
void loop() {
    
    delay(1000);
    // Get Temperature
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
        
    // Get Pressure
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(99082));
    Serial.println(" meters");
  
    Serial.println();

    delay(2000);
}