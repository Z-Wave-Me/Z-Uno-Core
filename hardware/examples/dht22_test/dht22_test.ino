#include "ZUNO_DHT.h"


DHT    dht22_sensor(11, DHT22);

void setup() {
  
  Serial.begin(115200);
  dht22_sensor.begin();
  
  Serial.println("\n **** Sketch is starting... ****\n");
  

}




void loop() {
  
  byte result;
  byte i;
  byte  raw_data[5]; 

  Serial.print("Millis:");
  Serial.println(millis());


  result = dht22_sensor.read(true); 
  Serial.print("DHT read result:");
  Serial.println(result);

  Serial.print("Raw data: { ");
  dht22_sensor.getRawData(raw_data);
  for(i=0;i<5;i++)
  {
    Serial.print(raw_data[i], HEX);
    Serial.print(" ");
  }
  Serial.println("} ");

  Serial.print("Temperature:");
  Serial.println(dht22_sensor.readTemperature());
  Serial.print("Humidity:");
  Serial.println(dht22_sensor.readHumidity()); 

  delay(3000);  

   

}
