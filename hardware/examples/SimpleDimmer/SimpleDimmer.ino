/*
 * That is a Simple Multilevel Dimmer example
 * It gives you the ability to control the 
 * built in LED brightness via Z-Wave commands
 */

// LED pin number
#define LED_PIN 13

// variable to store current dimmer value
byte lastSetDimmer;

// next macro sets up the Z-Uno channels
// in this example we set up 1 switch multilevel channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SWITCH_MULTILEVEL/
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_MULTILEVEL(getter, setter));

void setup() {
  pinMode(LED_PIN, OUTPUT); //set up LED pin as output
}
void loop() {
  // loop is empty, because all the control is over the Z-Wave
}

// setter function is called once a command comes 
// from the Z-Wave controller or over controlling device
void setter(byte value) {
  byte tempVariable;
  // value is a variable, holding a "new value"
  // which came from the controller or other Z-Wave device
  if (value > 99) {
    // by Z-Wave specification, this value can't be more then 99
    value = 99;
  }
  // but the LED brightness scale ranges from 0 to 255
  // we need to prepare our "value" for this new scale
  tempVariable = ((word)value)*255/99;

  // now we set the LED brightness
  analogWrite(1, ((long)value)*255/99);
  
  // let's save our value for the situation, when the controller will ask us about it
  lastSetDimmer = value;
}

// getter function is called once the controller
// or other device in the network asks Z-Uno about
// it's current level
byte getter(void) {
  // return previously saved (in getter()) value
  return lastSetDimmer;
}
