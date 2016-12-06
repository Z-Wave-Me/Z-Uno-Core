/*
 * This is a simple 2 chennel meter example
 * (c) Z-Wave.Me 2016
 */
 
#include <EEPROM.h>

// channel number
#define ZUNO_CHANNEL_NUMBER_COLD   1
#define ZUNO_CHANNEL_NUMBER_HOT    2

#define PIN_COLD    0
#define PIN_HOT     1

#define PIN_PULSE   13

#define EEPROM_ADDR             0x800
#define EEPROM_UPDATE_INTERVAL  120000

#define TICK_VALUE              10 // in liters


struct meter_data
{
    unsigned long ticks_cold;
    unsigned long ticks_hot;
    byte          crc8;
};

meter_data my_meter_data;

unsigned long last_update_millis = 0;
byte data_updated  = FALSE;

#define DEBOUNCE_COUNT  3
byte cold_triggered = 0;
byte cold_debounce = DEBOUNCE_COUNT;
byte hot_triggered = 0;
byte hot_debounce = DEBOUNCE_COUNT;



// next macro sets up the Z-Uno channels
// in this example we set up 1 meter channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SENSOR_MULTILEVEL/
ZUNO_SETUP_CHANNELS(ZUNO_METER(ZUNO_METER_TYPE_WATER, METER_RESET_ENABLE, ZUNO_METER_WATER_SCALE_METERS3, METER_SIZE_FOUR_BYTES, METER_PRECISION_THREE_DECIMALS, getterCold, resetterCold),
                    ZUNO_METER(ZUNO_METER_TYPE_WATER, METER_RESET_ENABLE, ZUNO_METER_WATER_SCALE_METERS3, METER_SIZE_FOUR_BYTES, METER_PRECISION_THREE_DECIMALS, getterHot, resetterHot));

#define MY_SERIAL Serial0

byte my_crc8(byte * data, byte count)
{
    byte result = 0xDF;

    while(count--)
    {
        result ^= *data;
        data++;
    }
    return result;
}
void update_meter_data()
{ 
    my_meter_data.crc8 = my_crc8((byte*)&my_meter_data, sizeof(meter_data) - 1);
    EEPROM.put(EEPROM_ADDR, &my_meter_data, sizeof(meter_data));
}
void setup() {
  MY_SERIAL.begin(115200);
  MY_SERIAL.println("Starting Meter...");

  // Dry contacts of meters connect to these pins
  pinMode(PIN_COLD, INPUT_PULLUP);
  pinMode(PIN_HOT, INPUT_PULLUP);

  // Get last meter values from EEPROM
  EEPROM.get(EEPROM_ADDR,  &my_meter_data, sizeof(meter_data));
  // Check data  
  if(my_crc8((byte*)&my_meter_data, sizeof(meter_data) - 1) != my_meter_data.crc8)
  {
      // Invalid data - reset all
      MY_SERIAL.println("Bad eeprom crc8 - init meter data");
      my_meter_data.ticks_cold = 0;
      my_meter_data.ticks_hot  = 0;
      update_meter_data();
            
  }
   
}



void loop() {


    if(!digitalRead(PIN_COLD))
    {
        if(!cold_triggered)
        {
          cold_debounce--;
          if(!cold_debounce)
          {
            my_meter_data.ticks_cold++;
            data_updated = true;
            cold_triggered = true;
            MY_SERIAL.println("Cold++");
            zunoSendReport(ZUNO_CHANNEL_NUMBER_COLD); 
          }
        }
    }
    else
    {
        cold_debounce = DEBOUNCE_COUNT;
        cold_triggered = false;

    }
    if(!digitalRead(PIN_HOT))
    {
        if(!hot_triggered)
        {
          hot_debounce--;
          if(!hot_debounce)
          {
            my_meter_data.ticks_hot++;
            data_updated = true;
            hot_triggered = true;
            MY_SERIAL.println("Hot++");
            zunoSendReport(ZUNO_CHANNEL_NUMBER_HOT); 
          }
        }

    }
    else
    {
        hot_debounce = DEBOUNCE_COUNT;
        hot_triggered = false;

    }
    // To save EEPROM from a lot of r/w operation 
    // write it once in EEPROM_UPDATE_INTERVAL if data was updated
    if(data_updated && ( millis() - last_update_millis ) > EEPROM_UPDATE_INTERVAL )
    {
        update_meter_data();
        data_updated = false;
        last_update_millis =  millis();
        MY_SERIAL.println("Updating EEPROM");
    }
    delay(100);
   
}


void resetterCold(byte v) {
  my_meter_data.ticks_cold = 0;
  update_meter_data();
  MY_SERIAL.println("Cold meter was reset");
}
void resetterHot(byte v) {
  my_meter_data.ticks_hot  = 0;
  update_meter_data();
  MY_SERIAL.println("Hot meter was reset");
}

unsigned long getterCold(void) {
  return my_meter_data.ticks_cold*TICK_VALUE;
}
unsigned long getterHot(void) {
  return my_meter_data.ticks_hot*TICK_VALUE;
}
