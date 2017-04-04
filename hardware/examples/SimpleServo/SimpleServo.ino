/*
 * That is a simple servo test. It just drives a single servo
 */

#include <ZUNO_SERVO.h> 

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC (Serial)
#define MY_SERIAL Serial

// Connect servo PWM to Z-Uno digital pin 12 
ServoController servo(12);

// variable to store current dimmer value
byte lastSetDimmer;

// next macro sets up the Z-Uno channels
// in this example we set up 1 switch multilevel channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SWITCH_MULTILEVEL/
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_MULTILEVEL(getter, setter));
ZUNO_SETUP_DEBUG_MODE(DEBUG_ON);

void setup() {
  servo.begin();
  MY_SERIAL.begin(115200);
  MY_SERIAL.println("Starting...");

}

void setter(byte value) {
  byte tempVariable;
  // value is a variable, holding a "new value"
  // which came from the controller or other Z-Wave device
  if (value > 99) {
    // by Z-Wave specification, this value can't be more then 99
    value = 99;
  }
  // but the Servo angle scale ranges from 0 to 180
  // we need to prepare our "value" for this new scale
  tempVariable = ((word)value)*180/99;
  // now we set the Servo position
  servo.setValue(tempVariable);
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


void loop() {

  MY_SERIAL.print("millis:");
  MY_SERIAL.println(millis());
  MY_SERIAL.print("current value:");
  MY_SERIAL.println(lastSetDimmer);

  delay(1000);
  
}
