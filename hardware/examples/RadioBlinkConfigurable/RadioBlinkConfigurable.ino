/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Z-Uno has an on-board LED you can control. It is attached to digital pin 13.
  LED_BUILTIN is an internal Define which can be used to access this LED.
  This example demonstrates the simple blink programm, where the blink interval can be changed from the Z-Wave controller.
  More information on http://z-uno.z-wave.me/
  You can cofigure duration using parameter #64.

 */

//initial period of the blink is 1 second
byte dimmerValue=100;
dword coef;
byte changed = FALSE;
byte count = 0;
byte type = 0;

//Next line sets up Z-Uno channels. In this case it adds the Switch Multilevel channel
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_MULTILEVEL(getSwitchMultilevelValue,setSwitchMultilevelValue));
// We have to define handler for configuration parameters
ZUNO_SETUP_CFGPARAMETER_HANDLER(config_parameter_changed);
// the setup function runs once, when you press reset or power the board
void setup() {
  // set digital pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  dimmerValue = 100;
  // load value of the first user-defined configuration parameter to variable coef
  // The first user defined parameter is #64, the second is #65 etc
  // We can use up to 32 user-defined parameters 
  zunoLoadCFGParam(64,&coef);
  // In case we don't have right value here
  if(coef > 20)
     coef = 20;
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(dimmerValue*byte(coef));           // wait for timeout
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(dimmerValue*byte(coef));           // wait for timeout

}
void config_parameter_changed(byte param, dword * value)
{
    // Here we get the new vaue of our prameters
    if(param == 64) // The first user-defined parameter 
    {
      coef = *value;  
    }
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
