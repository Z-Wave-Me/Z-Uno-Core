/**************************************************************************/
/*! 
    @file     Adafruit_MCP4725.h
    @author   K. Townsend (Adafruit Industries)
	@license  BSD (see license.txt)
	
	This is a library for the Adafruit MCP4725 breakout board
	----> http://www.adafruit.com/products/935
	
	Adafruit invests time and resources providing this open source code, 
	please support Adafruit and open-source hardware by purchasing 
	products from Adafruit!

	@section  HISTORY

    v1.0  - First release
	some changes for Z-Uno by m.p. 22.11.2016
    @file     ZUNO_MCP4725.h

*/
/**************************************************************************/

#include <Wire.h>

#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

class ZUNO_MCP4725{
 public:
  ZUNO_MCP4725();
  void begin(uint8_t a);  
  void setVoltage( uint16_t output, bool writeEEPROM );

 private:
  uint8_t _i2caddr;
};
