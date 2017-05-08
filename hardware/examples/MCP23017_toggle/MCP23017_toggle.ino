#include <Wire.h>
#include "ZUNO_MCP23017.h"

// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect SCL to i2c clock - on Z-Uno thats Digital 9
// Connect SDA to i2c data - on Z-Uno thats Digital 10
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Output #0 is on pin 21 so connect an LED or whatever from that to ground

ZUNO_MCP23017 mcp;
  
void setup() {  
  mcp.begin();      // use default address 0

  mcp.pinMode(0, OUTPUT);
}


// flip the pin #0 up and down

void loop() {
  delay(100);

  mcp.digitalWrite(0, HIGH);

  delay(100);

  mcp.digitalWrite(0, LOW);
}