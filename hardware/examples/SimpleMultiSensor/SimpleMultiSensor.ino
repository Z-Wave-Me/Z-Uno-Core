/*
 * That is a Simple Sensor Multilevel example
 * It measures the value on the potentiometer
 * And sends report to the controller if changed
 */
 
// LED pin number
#define LED_PIN 13
// Potentiometer pin number
#define POT_PIN 6

// channel number
#define ZUNO_CHANNEL_NUMBER_ONE   1

// Last saved potentiometer value
byte lastValue;

// next macro sets up the Z-Uno channels
// in this example we set up 1 sensor multilevel channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SENSOR_MULTILEVEL/
ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_MULTILEVEL_GENERAL_PURPOSE(getter));

void setup() {
  pinMode(LED_PIN, OUTPUT); // setup pin as output
  pinMode(POT_PIN, INPUT); //setup potentiometer pin as input
}
void loop() {
  // read potemtiometer value and save it inside a variable
  byte currentValue = (byte) (analogRead(2) / 4);

  // if the value is different then the previously measured one
  // save it and send a report
  if (currentValue != lastValue) {
    // save the value
    lastValue = currentValue;
    // send report to the controller
    zunoSendReport(ZUNO_CHANNEL_NUMBER_ONE);  
  }
}

// function, which returns the previously saved potentiometer value
// this function runs only once the controller asks
byte getter(void) {
  byte tempVariable;
  tempVariable = (byte)((((word) lastValue)*100)/0xff);
  return tempVariable;
}
