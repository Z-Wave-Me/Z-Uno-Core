/*
 * That is a Simple Sensor Motion example
 * It watches over the built in button state
 * And sends report to the controller if detects motion
 */

// built in LED number
#define LED_PIN     13

// motion trigger pin
#define MOTION_PIN  12

// channel number
#define ZUNO_CHANNEL_NUMBER_ONE   1

// next macro sets up the Z-Uno channels
// in this example we set up 1 sensor binary channel
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SENSOR_BINARY/
ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_MOTION, getter));

// next macro sets up the Z-Uno frequency
ZUNO_SETUP_FREQUENCY(ZUNO_FREQ_RU);

// variable to store current motion state
byte lastSensorValue = 0;

void setup(){
  //enable usb printing
  Serial.begin();
  pinMode(LED_PIN, OUTPUT); // set LED pin as output
  pinMode(MOTION_PIN,INPUT); // set motion pin as input
}
void loop(){
  // sample current motion state
  byte currentSensorValue = digitalRead(MOTION_PIN);
  
  if (currentSensorValue != lastSensorValue) { // if motion state changes
    lastSensorValue = currentSensorValue; // save new state
    zunoSendReport(ZUNO_CHANNEL_NUMBER_ONE); // send report over the Z-Wave to the controller
    if (currentSensorValue == HIGH){      // is there is motion
      Serial.println("Motion detected");  // send message over USB
      digitalWrite(LED_PIN, HIGH);        // turn LED on
    } else {                              // if motion ended
      Serial.println("Motion stopped");   // send message over USB
      digitalWrite(LED_PIN, LOW);         // turn LED off
    }
  }
}

// function, which returns the previously saved button state
// this function runs only once the controller asks
byte getter(){
  if (lastSensorValue == 1) {    // if motion is detected
    return 0xff;                 // return "Triggered" state to the controller
  } else {                       // if motion stopped
    return 0;                    // return "Idle" state to the controller
  }
}

