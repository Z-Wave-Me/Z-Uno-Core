#include <ZUNO_LPS331.h>

// создаём объект для работы со сканером
LPS331 lps331;

#define MY_SERIAL Serial0

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