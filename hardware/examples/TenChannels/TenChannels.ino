#define LedPin1 9
#define LedPin2 10
#define LedPin3 11
#define LedPin4 14
#define LedPin5 15
#define LedPin6 16
#define MotionPin 12
#define MoistPin A2
#define TemperaturePin A3
#define DoorPin 19
#define led 13;
byte currentLEDValue1;
byte currentLEDValue2;
byte currentLEDValue3;
byte lastSetDimmer1;
byte lastSetDimmer2;
byte lastSetDimmer3;
byte lastSensorValue = 0;
byte lastDoorValue;
byte temperature;
byte MoistNew;
byte MoistValue;
byte CurrentMoist;
byte CurrentDoorValue;
//датчик освещенности
//любой бинари сенсор
ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_ALWAYS_AWAKE);
ZUNO_SETUP_CHANNELS(
  ZUNO_SWITCH_BINARY(getterSwitch1, setterSwitch1),
  ZUNO_SWITCH_BINARY(getterSwitch2, setterSwitch2),
  ZUNO_SWITCH_BINARY(getterSwitch3, setterSwitch3),
  ZUNO_SWITCH_MULTILEVEL(getterDimm1, setterDimm1),
  ZUNO_SWITCH_MULTILEVEL(getterDimm2, setterDimm2),
  ZUNO_SWITCH_MULTILEVEL(getterDimm3, setterDimm3),
  ZUNO_SENSOR_BINARY_MOTION(getterMotion),
  ZUNO_SENSOR_MULTILEVEL_HUMIDITY(getterMoisture),
  ZUNO_SENSOR_MULTILEVEL_TEMPERATURE(getterTemperature),
  ZUNO_SENSOR_BINARY_DOOR_WINDOW(getterDoor)
);
  


void setup() {
  pinMode(LedPin1, OUTPUT);
  pinMode(LedPin2, OUTPUT);
  pinMode(LedPin3, OUTPUT);
  pinMode(LedPin4, OUTPUT);
  pinMode(LedPin5, OUTPUT);
  pinMode(LedPin6, OUTPUT);
  pinMode(MotionPin, INPUT);
  pinMode(MoistPin, OUTPUT);
  pinMode(DoorPin, INPUT_PULLUP);
  pinMode(TemperaturePin, OUTPUT);
}

void loop() {
  byte CurrentSensorValue = digitalRead(MotionPin); // датчик движения
  
  if (CurrentSensorValue != lastSensorValue) { 
  lastSensorValue = CurrentSensorValue; 
   }
  CurrentMoist = analogRead(MoistPin);// влажность
      if (MoistNew != CurrentMoist) {
   MoistNew = CurrentMoist;
   }
    CurrentDoorValue = digitalRead(DoorPin); // геркон
  
  if (CurrentDoorValue != lastDoorValue) { 
    lastDoorValue = CurrentDoorValue;
    }
}


 void setterSwitch1(byte value1) {
   if (value1 > 0) {
     digitalWrite (LedPin1, HIGH);
   } else {
     digitalWrite (LedPin1, LOW);
   }
   currentLEDValue1 = value1;
   
}
byte getterSwitch1 (){
  return  currentLEDValue1;
  
 }
 
 void setterSwitch2(byte value2) {

   if (value2 > 0){
    digitalWrite(LedPin2, HIGH);
  } else {
    digitalWrite(LedPin2, LOW);
  }
  currentLEDValue2 = value2;
  
}
 byte getterSwitch2 (){
  return  currentLEDValue2;
 }

 void setterSwitch3(byte value3) {

  if (value3 > 0){
    digitalWrite(LedPin3, HIGH);
  } else {
    digitalWrite(LedPin3, LOW);
  }
  currentLEDValue3 = value3;
  (1000);
}
 byte getterSwitch3 (){
  return currentLEDValue3;
 }
 void setterDimm1(byte value4) {
  byte tempVariable1;
  if (value4 > 99) {
    value4 = 99;
  }
  tempVariable1 = ((word)value4)*255/99;
  analogWrite(1, ((long)value4)*255/99);
  lastSetDimmer1 = value4;
  
}

byte getterDimm1(void) {
  return lastSetDimmer1;
}
void setterDimm2(byte value5) {
  byte tempVariable2;
  if (value5 > 99) {
    value5 = 99;
  }
  tempVariable2 = ((word)value5)*255/99;
  analogWrite(2, ((long)value5)*255/99);
  lastSetDimmer2 = value5;
 
}

byte getterDimm2(void) {
  return lastSetDimmer2;
}
void setterDimm3(byte value6) {
  byte tempVariable3;
  if (value6 > 99) {
    value6 = 99;
  }
  tempVariable3 = ((word)value6)*255/99;
  analogWrite(3, ((long)value6)*255/99);
  lastSetDimmer3 = value6;
  
}
byte getterDimm3(void) {
  return lastSetDimmer3;
}

byte getterMotion(void) {
  if (lastSensorValue == 1) {
    return 0xff;
  } else {
    return 0;
  }
  
}
byte getterMoisture(void) {
byte tempLevel = CurrentMoist / 9; 
  if (tempLevel > 100) tempLevel = 100;
  return tempLevel;
}
byte getterTemperature(void) {
  temperature = (analogRead(TemperaturePin) * 3.3 * 100)/1024 - 50;
   return temperature;
}
byte getterDoor(void) {
  if (lastDoorValue == 1) {
    return 0xff;
  } else {
    return 0;
  }
  
}
