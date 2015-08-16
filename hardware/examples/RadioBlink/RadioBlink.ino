/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Z-Uno has an on-board LED you can control. It is attached to digital pin 13.
  LED_BUILTIN is an internal Define which can be used to access this LED.
  This example demonstrates the simple blink programm, where the blink interval can be changed from the Z-Wave controller.
  More information on http://z-uno.z-wave.me/

 */

//initial period of the blink is 1 second
byte dimmerValue=100;

//Next line sets up Z-Uno channels. In this case it adds the Switch Multilevel channel
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_MULTILEVEL(getSwitchMultilevelValue,setSwitchMultilevelValue));

// next macro sets up the Z-Uno frequency
ZUNO_SETUP_FREQUENCY(ZUNO_FREQ_EU);

// the setup function runs once, when you press reset or power the board
void setup() {
  // set digital pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  dimmerValue = 100;
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(dimmerValue*10);           // wait for timeout
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(dimmerValue*10);           // wait for timeout
}

//The setSwitchMultilevelValue runs only when a new value comes from the controller by Z-Wave
void setSwitchMultilevelValue(byte newValue) {
  //save new value in a variable
  dimmerValue = newValue;
}

//The setSwitchMultilevelValue runs only when the controller asks for the current blink rate by Z-Wave
byte getSwitchMultilevelValue(void) {
  //return previously saved value
  return dimmerValue;
}
