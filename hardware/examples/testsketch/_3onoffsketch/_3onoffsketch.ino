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
byte currentLEDvalue1;
byte currentLEDvalue2;
byte currentLEDvalue3;
byte lastSetDimmer1;
byte lastSetDimmer2;
byte lastSetDimmer3;
byte lastSensorValue = 0;
byte lastDoorValue;
byte temperature;
byte MoistNew;
byte CurrentDoorValue;
byte lastTemperature;
byte CurrentTemperature;
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
  ZUNO_SENSOR_MULTILEVEL_MOISTURE(getterMoisture),
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
  pinMode(MoistPin, INPUT);
  pinMode(DoorPin, INPUT_PULLUP);
  pinMode(TemperaturePin, INPUT);
}

void loop() {
 word CurrentMoist;
 word CurrentTemperature;
 /*byte CurrentSensorValue = digitalRead(MotionPin); // датчик движения  
       if (CurrentSensorValue != lastSensorValue) { 
        lastSensorValue = CurrentSensorValue; 
        zunoSendReport(7);
   }
      CurrentMoist = analogRead(MoistPin);// влажность
       if (MoistNew != CurrentMoist) {
        MoistNew = CurrentMoist;
         zunoSendReport(8);
   }
      CurrentDoorValue = digitalRead(DoorPin); // геркон  
       if (CurrentDoorValue != lastDoorValue) { 
        lastDoorValue = CurrentDoorValue;
         zunoSendReport(10);
   }*/
      CurrentTemperature = ((long)(analogRead(TemperaturePin) * 330)/1024 - 50);
       if (CurrentTemperature != lastTemperature) {
        lastTemperature = CurrentTemperature;

        // zunoSendReport(9);
   }
}
 void setterSwitch1(byte value) {
   if (value > 0) {
     digitalWrite (LedPin1, HIGH);
   } else {
     digitalWrite (LedPin1, LOW);
   }
      currentLEDvalue1 = value;
   
}
byte getterSwitch1 (){
  return  currentLEDvalue1;
  
 }
 
 void setterSwitch2(byte value) {

   if (value > 0){
    digitalWrite(LedPin2, HIGH);
  } else {
    digitalWrite(LedPin2, LOW);
  }
      currentLEDvalue2 = value;
  
}
 byte getterSwitch2 (){
  return  currentLEDvalue2;
 }

 void setterSwitch3(byte value) {

  if (value > 0){
    digitalWrite(LedPin3, HIGH);
  } else {
    digitalWrite(LedPin3, LOW);
  }
  currentLEDvalue3 = value;
  
}
 byte getterSwitch3 (){
  return currentLEDvalue3;
 }
 void setterDimm1(byte value) {
  if (value > 99) {
    value = 99;
  }
  analogWrite(1, ((long)value)*255/99);
  lastSetDimmer1 = value;
  
}

byte getterDimm1(void) {
  return lastSetDimmer1;
}
void setterDimm2(byte value) {
  if (value > 99) {
    value = 99;
  }
  analogWrite(2, ((long)value)*255/99);
  lastSetDimmer2 = value;
 
}

byte getterDimm2(void) {
  return lastSetDimmer2;
}
void setterDimm3(byte value) {
  if (value > 99) {
    value = 99;
  }
  analogWrite(3, ((long)value)*255/99);
  lastSetDimmer3 = value;
  
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
 byte tempLevel = MoistNew / 9; 
  if (tempLevel > 100) tempLevel = 100;
  return tempLevel;
}
byte getterTemperature(void) {
   return CurrentTemperature;
}
byte getterDoor(void) {
  if (lastDoorValue == 1) {
    return 0xff;
  } else {
    return 0;
  }
}
