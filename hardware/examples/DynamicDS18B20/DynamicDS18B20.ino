// Multiple temperature sensors DS18B20
#include "ZUNO_DS18B20.h"
#define DS18B20_BUS_PIN 11                  // Pin to which 1-Wire bus is connected
#define MAX_SENSORS     32                  // Number of DS18B20 sensors supported (equals to maximum number of channels for Z-Uno) 
#define MY_SERIAL       Serial0             // Serial interface that we use for debug

OneWire ow(DS18B20_BUS_PIN);                // Software 1-Wire BUS
DS18B20Sensor ds18b20(&ow);                 // connect DS18B20 class to it

#define ADDR_SIZE 8                         // Size of address of devices on 1-wire bus
byte addresses[ADDR_SIZE * MAX_SENSORS];        // Here we store all the scanned addresses
#define ADDR(i) (&addresses[i * ADDR_SIZE]) // Macro to simplify our life
byte number_of_sensors;                     // Number of sensors found
word temperature[MAX_SENSORS];              // Here we store temperatures

void dumpData(BYTE * data)
{
  byte i;
  for(i=0;i<16;i++)
  {
    Serial0.print(data[i], HEX);
    Serial0.print(' ');
  }
  Serial0.print('\n');
}
byte tmp_dmp[16];
void setup() {
  // DBG output
  //MY_SERIAL.end();
  MY_SERIAL.begin(115200);
  /*
  memcpy(tmp_dmp, syscall_data.param_data, 16);
  Serial0.print("Last Call data:");
  dumpData(tmp_dmp);*/
  MY_SERIAL.println("Scanning DS18B20 sensors...");
  // Scanning sensors on the bus every time we starting a sketch
  number_of_sensors = ds18b20.findAllSensors(addresses);
  // DBG output
  MY_SERIAL.print("Found: ");
  MY_SERIAL.println(number_of_sensors);

  if(number_of_sensors > MAX_SENSORS)
    number_of_sensors = MAX_SENSORS;
  // Setting up Z-Uno channels
  // We do id dynamically... 
  // You have to exclude/include your Z-Uno to this take effect on the controller side
  ZUNO_START_CONFIG();
  byte i;
  for (i = 0; i < number_of_sensors; i++)
  {
    // Each channel is temperature sensor 
    // with 2 decimals precision (ex. 25.45 )
    ZUNO_ADD_CHANNEL(ZUNO_SENSOR_MULTILEVEL_CHANNEL_NUMBER, 
                     ZUNO_SENSOR_MULTILEVEL_TYPE_TEMPERATURE, 
                     SENSOR_MULTILEVEL_PROPERTIES_COMBINER(
                            SENSOR_MULTILEVEL_SCALE_CELSIUS, 
                            SENSOR_MULTILEVEL_SIZE_TWO_BYTES, 
                            SENSOR_MULTILEVEL_PRECISION_TWO_DECIMALS));
  }
  // Commit configuration that we made...
  ZUNO_COMMIT_CONFIG();
  
  

}

void loop() {
  byte i;
  for (i = 0; (i < number_of_sensors); i++) 
  {
    // Read temperature
    float ct =  ds18b20.getTemperature(ADDR(i));
    temperature[i] = ct * 100; // Convert temperature to fixed point (d)+.dd
    // Sending report
    zunoSendReport(i + 1); // Channels start from 1
    // DBG output
    MY_SERIAL.print("Temperaure mesured with #");
    MY_SERIAL.print(i);
    MY_SERIAL.print(" sensor: ");
    MY_SERIAL.println(ct);
    
  }
  // We have to wait 30 seconds 
  // It's a requirement of Z-Wave protocol
  delay(30000);
}

// Universal handler for all the channels
void zunoCallback(void)
{
    // See callback_data variable 
    // We use word params for all 
    // We use zero based index of the channel instead of typical 
    // Getter/Setter index of Z-Uno. 
    // See enum ZUNO_CHANNEL*_GETTER/ZUNO_CHANNEL*_SETTER in ZUNO_Definitions.h 
    byte index = callback_data.type;
    index >>= 1;
    index --;
    callback_data.param.wParam = temperature[index];
}
