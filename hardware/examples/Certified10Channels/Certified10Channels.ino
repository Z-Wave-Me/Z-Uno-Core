/* 
 * This scretch was certified by the Z-Wave Alliance as one of the two reference Z-Uno sketches.
 * 
 * 3 switches
 * 3 dimmers
 * 1 motion sensor
 * 1 temperature sensor
 * 1 luminance sensor
 * 1 door sensor
 * 
 */

// Pins definitions
#define LedPin1 9
#define LedPin2 10
#define LedPin3 11
#define LedPin4 PWM2
#define LedPin5 PWM3
#define LedPin6 PWM4
#define MotionPin 12
#define LumiPin A2
#define TemperaturePin A3
#define DoorPin 19

// Global variables to store data reported via getters
byte switchValue1 = 0;
byte switchValue2 = 0;
byte switchValue3 = 0;
byte dimValue1 = 0;
byte dimValue2 = 0;
byte dimValue3 = 0;
byte lastMotionValue = 0;
word lastLumiValue = 0;
byte lastTemperatureValue = 0;
byte lastDoorValue = 0;
word relaxMotion = 0;

ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_ALWAYS_AWAKE);

// In group 1 send Basic Set from Door Sensor
ZUNO_SETUP_ASSOCIATIONS(ZUNO_ASSOCIATION_GROUP_SET_VALUE);

// Set up 10 channels
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getterSwitch1, setterSwitch1),
  ZUNO_SWITCH_BINARY(getterSwitch2, setterSwitch2),
  ZUNO_SWITCH_BINARY(getterSwitch3, setterSwitch3),
  ZUNO_SWITCH_MULTILEVEL(getterDim1, setterDim1),
  ZUNO_SWITCH_MULTILEVEL(getterDim2, setterDim2),
  ZUNO_SWITCH_MULTILEVEL(getterDim3, setterDim3),
  ZUNO_SENSOR_BINARY_MOTION(getterMotion),
  ZUNO_SENSOR_MULTILEVEL(ZUNO_SENSOR_MULTILEVEL_TYPE_LUMINANCE, SENSOR_MULTILEVEL_SCALE_PERCENTAGE_VALUE, SENSOR_MULTILEVEL_SIZE_ONE_BYTE, SENSOR_MULTILEVEL_PRECISION_ZERO_DECIMALS, getterLuminance),
  ZUNO_SENSOR_MULTILEVEL_TEMPERATURE(getterTemperature),
  ZUNO_SENSOR_BINARY_DOOR_WINDOW(getterDoor)
);

void setup() {
  // set up I/O pins. Analog and PWM will be automatically set up on analogRead/analogWrite functions call
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);
  pinMode(LedPin3, OUTPUT);
  pinMode(MotionPin, INPUT_PULLUP);
  pinMode(DoorPin, INPUT_PULLUP);
}

void loop() {
  byte currentMotionValue;
  word currentLumiValue;
  byte currentTemperatureValue;
  byte currentDoorValue;

  // Trigger motion and wait for relax (about 5 sec) before report idle
  currentMotionValue = !digitalRead(MotionPin);
  if (currentMotionValue) {
    if (relaxMotion == 0) {
      lastMotionValue = 1;
      zunoSendReport(7);
      zunoSendToGroupSetValueCommand(2, 255);
    }
    relaxMotion = 1900; // impirical for ~5 sec relax time
  }
  if (lastMotionValue == 1 && relaxMotion == 0) {
    lastMotionValue = 0; 
    zunoSendReport(7);
    zunoSendToGroupSetValueCommand(2, 0);
  }
  if (relaxMotion) relaxMotion--;

  // Luminosity
  currentLumiValue = 100 - analogRead(LumiPin)*25/256;
  if ((currentLumiValue > (lastLumiValue + 5)) || (currentLumiValue < (lastLumiValue - 5))) {
    lastLumiValue = currentLumiValue;
    zunoSendReport(8);
  }

  // TMP36 analog temperature sensor
  currentTemperatureValue = analogRead(TemperaturePin) * 40 / 124 - 50; // from TMP36 spec (40/124 ~= 3.3*100/1024)
  if ((currentTemperatureValue > (lastTemperatureValue + 1)) || (currentTemperatureValue < (lastTemperatureValue - 1))) {
    lastTemperatureValue = currentTemperatureValue;
    zunoSendReport(9);
  }

  // Door/Window sensor
  currentDoorValue = digitalRead(DoorPin); 
  if (currentDoorValue != lastDoorValue) { 
    lastDoorValue = currentDoorValue;
    zunoSendReport(10);
  }
}

// Getters and setters

void setterSwitch1(byte value) {
  digitalWrite(LedPin1, (value > 0) ? HIGH : LOW);
  switchValue1 = value;
}

byte getterSwitch1(){
  return switchValue1;
}

 void setterSwitch2(byte value) {
  digitalWrite(LedPin2, (value > 0) ? HIGH : LOW);
  switchValue2 = value;
}

byte getterSwitch2(){
  return switchValue2;
}

void setterSwitch3(byte value) {
  digitalWrite(LedPin3, (value > 0) ? HIGH : LOW);
  switchValue3 = value;
}

byte getterSwitch3(){
  return switchValue3;
}

void setterDim1(byte value) {
  if (value > 99) value = 99;
  analogWrite(LedPin4, ((word)value)*255/99);
  dimValue1 = value;
}

byte getterDim1(void) {
  return dimValue1;
}

void setterDim2(byte value) {
  if (value > 99) value = 99;
  analogWrite(LedPin5, ((word)value)*255/99);
  dimValue2 = value;
}

byte getterDim2(void) {
  return dimValue2;
}

void setterDim3(byte value) {
  if (value > 99) value = 99;
  analogWrite(LedPin6, ((word)value)*255/99);
  dimValue3 = value;
}

byte getterDim3(void) {
  return dimValue3;
}

byte getterMotion(void) {
  return lastMotionValue ? 0xff : 0;
}

word getterLuminance(void) {
  return lastLumiValue;
}

byte getterTemperature(void) {
  return lastTemperatureValue;
}

byte getterDoor(void) {
  return lastDoorValue ? 0xff : 0;
}

