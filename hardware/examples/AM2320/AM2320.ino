#include <ZUNO_AM2320.h>
#define MY_SERIAL Serial0

AM2320 am2320;

void setup() {
  am2320.begin();
  MY_SERIAL.begin(115200);
  MY_SERIAL.println("Sketch is starting...");
}

void loop() {

  MY_SERIAL.println(millis());
  // put your main code here, to run repeatedly:
  float temp = am2320.getTemperature();
  float hum = am2320.getHumidity();

  byte i;
  byte * raw = am2320.rawData();
  for(i=0;i<8;i++)
  {
       MY_SERIAL.print(" ");
      MY_SERIAL.print(raw[i],HEX);
  }
  MY_SERIAL.println();
  
  MY_SERIAL.print("err_code:");
  MY_SERIAL.println(am2320.getLastError());

  MY_SERIAL.print("Temp: ");
  MY_SERIAL.println(temp);

  MY_SERIAL.print("Hum: ");
  MY_SERIAL.println(hum);
  

  delay(5000);
  
}
