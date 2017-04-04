/*
 *  Sketch that scans 1-wire bus and prints all found
 *   DS18B20 sensors ROMS and values.
 *         (c) Z-Wave.Me 2016 
 */

#include <ZUNO_OneWire.h>
#include <ZUNO_DS18B20.h>

// LED pin number
#define BUS_PIN 12
#define ROM_SIZE 8
#define MAX_SENSOR 8


OneWire ow(BUS_PIN);
DS18B20Sensor ds18b20(&ow);



byte sensor_roms[ROM_SIZE*MAX_SENSOR];
byte number_of_sensors;

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC (Serial)
#define MY_SERIAL Serial

#define ROM_DATA(index) (&sensor_roms[index*ROM_SIZE])
void printROM(byte * rom)
{
    byte i;
    for(i=0; i<8; i++)
    {
      MY_SERIAL.print(" ");
      MY_SERIAL.print(rom[i], HEX);
    }   
}

void setup() {

	MY_SERIAL.begin(115200);
  MY_SERIAL.println("Sketch is starting...");
  number_of_sensors = ds18b20.findAllSensors(sensor_roms);
  MY_SERIAL.print("FOUND ");
  MY_SERIAL.print(number_of_sensors);
  MY_SERIAL.println(" DS18B20 sensor(s)");
  byte i;

  for(i=0; i<number_of_sensors; i++ )
  {
       MY_SERIAL.print("\nSensor ");
       MY_SERIAL.print(i);
       MY_SERIAL.print(" ROM: ");
       printROM(ROM_DATA(i));      

  }  


     
}

void loop() {

  byte i;	

  MY_SERIAL.println("\n\n    Polling temperature sensors...\n");
    
  for(i=0;i<number_of_sensors;i++)
  {
    MY_SERIAL.print("\n Sensor [");
    MY_SERIAL.print(i);
    MY_SERIAL.print("] ");
    MY_SERIAL.print(ds18b20.getTemperature(ROM_DATA(i)));
    MY_SERIAL.print("  *C");
    
  }
  

  delay(3000);
}
