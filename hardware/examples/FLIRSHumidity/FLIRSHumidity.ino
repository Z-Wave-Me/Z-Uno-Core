/*
 * Example of FLIRS-sensor
 * It measures temperatuture and humidity via most known DHT22 sensor.
 * You have to connect sensor DATA pin to pin11 of Z-Uno. 
 * The sensor could be wake up two ways:
 *   -- By radio, when you send a get request
 *   -- By the WUT-timer. In this case sensor reports 
 *      its values unsolicited
 */
 
#include <ZUNO_DHT.h>

// We make two channels in terms of Z-Wave.
// The first channel is temperature sensor.
// The second channel is humidity sensor. 

ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_TEMPERATURE, 
                                          SENSOR_MULTILEVEL_SCALE_CELSIUS,
                                          SENSOR_MULTILEVEL_SIZE_TWO_BYTES, 
                                          SENSOR_MULTILEVEL_PRECISION_ONE_DECIMAL, 
                                          temperature_getter),
                    ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_RELATIVE_HUMIDITY, 
                                          SENSOR_MULTILEVEL_SCALE_PERCENTAGE_VALUE, 
                                          SENSOR_MULTILEVEL_SIZE_TWO_BYTES, 
                                          SENSOR_MULTILEVEL_PRECISION_ONE_DECIMAL, 
                                          humidity_getter));

// next macro sets up the sleeping mode
// device will wake up by user request and regulary listening for packets
ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_FREQUENTLY_AWAKE);
// Setting up battery capacity according to millivolts on 3V pin of Z-Uno
// 2650 = 0%
// 3100 = 100%
ZUNO_SETUP_BATTERY_LEVELS(2650, 3100);

// Constract object to interact with DHT22 sensor.
DHT        dht22_sensor(11, DHT22);

// We can use here only UART(Serial0/Serail1) because we make all tasks 
// inside the setup function and we create a FLIRS device
#define MY_SERIAL Serial0
#define DEBUG     0
// Waking up by timer
// in seconds. 0= Disable this feature
#define WAKEUP_BEAMS_TIME 180 // every 2 minutes measure and send reports  

int humidity;
int temperatuture;
void setup() {
  
  dht22_sensor.begin();
  temperatuture = dht22_sensor.readTemperatureC10(); 
  humidity      = dht22_sensor.readHumidityH10();

  if(zunoGetWakeReason() == ZUNO_WAKEUP_REASON_WUT)
  {
    // Send reports for both channels in case 
    // device was woke up by timer 
    // You can add here your logic to check histeresis of these values...
    zunoSendReport(1);
    zunoSendReport(2);
     
  }
  // Set a timer to wakeup the device 
  zunoSetBeamCountWU(WAKEUP_BEAMS_TIME);

  #if DEBUG
  MY_SERIAL.begin(115200);
  MY_SERIAL.print("Temperaure:");
  MY_SERIAL.println(temperatuture);
  MY_SERIAL.print("Humidity:");
  MY_SERIAL.println(humidity);
  #endif


}

int temperature_getter()
{
  return temperatuture;
}
int humidity_getter()
{
  return humidity;
}

void loop() {
  // Send device to sleep ANYWAY )
  zunoSendDeviceToSleep();  
}
