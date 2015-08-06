/*
 * That is a Simple Sensor Multilevel example
 * It measures the value on the potentiometer
 * And sends report to the controller if changed
 */
 
// LED pin number
#define LED_PIN 13


// Last saved LED value
byte currentLEDValue;

// next macro sets up the Z-Uno channels
// in this example we set up 1 switch binary channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SWITCH_BINARY/
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(getter, setter));

// next macro sets up the Z-Uno frequency
ZUNO_SETUP_FREQUENCY(ZUNO_FREQ_RU);

void setup() {
  pinMode(LED_PIN, OUTPUT); // setup pin as output
}

void loop() { 
  // loop is empty, because all the control comes over the Z-Wave
}

// function, which sets new relay state
// this function runs only once the controller sends new value
 void setter (byte value) {
  // value is a variable, holding a "new value"
  // which came from the controller or other Z-Wave device
  if (value > 0) {    // if greater then zero
    digitalWrite (LED_PIN, HIGH); //turn LED on
  } else {            // if equals zero
    digitalWrite(LED_PIN, LOW);   //turn LED off
  } 
  // let's save our value for the situation, when the controller will ask us about it
  currentLEDValue = value;
}

// function, which returns the previously saved relay value
// this function runs only once the controller asks
byte getter (){
  return currentLEDValue;
}

