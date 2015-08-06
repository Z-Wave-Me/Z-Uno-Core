/*
 * This example shows basic Switch Binary functionality
 * Control an external relay on pin 13
 */

// pin number, where relay is connected
#define RELAY_PIN     13

// variable to store current relay state
byte lastSetValue;

// next macro sets up the Z-Uno channels
// in this example we set up 1 switch binary channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SWITCH_BINARY/
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(getter, setter));

// next macro sets up the Z-Uno frequency
ZUNO_SETUP_FREQUENCY(ZUNO_FREQ_RU);
           
void setup() {
  pinMode(RELAY_PIN, OUTPUT); // set up relay pin as output
}

void loop() {
  // loop is empty, because all the control comes over the Z-Wave
}


// function, which returns the previously saved relay value
// this function runs only once the controller asks
byte getter() {
    return lastSetValue;
}


// function, which sets new relay state
// this function runs only once the controller sends new value
void setter(byte newValue) {
  // newValue is a variable, holding a "value"
  // which came from the controller or other Z-Wave device
  if (newValue > 0) { // if greater then zero
    digitalWrite(RELAY_PIN, HIGH); //turn relay on
  } else {            // if equals zero
    digitalWrite(RELAY_PIN, LOW); //turn relay off
  }

  // save the new value in a variable
  lastSetValue = newValue;
}
