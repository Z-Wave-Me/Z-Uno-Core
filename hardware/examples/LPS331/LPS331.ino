#include <ZUNO_LPS331.h>

// Creating a sensor object...
LPS331 lps331;

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC
#define MY_SERIAL Serial

void setup() {
  
  
  MY_SERIAL.begin(115200);

  lps331.begin();
}


void loop() {
  
  MY_SERIAL.print("loop:");
  MY_SERIAL.println(millis());

  MY_SERIAL.print("Temperature: ");
  MY_SERIAL.print(lps331.readTemperatureC());
  MY_SERIAL.println(" *C");

  MY_SERIAL.print("Pressure:");
  MY_SERIAL.print(lps331.readPressurePascals());
  MY_SERIAL.println(" Pa");
  

  delay(1000);
  
}