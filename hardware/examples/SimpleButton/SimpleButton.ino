/*
 * That is a Simple button example
 * Z-Uno has a built in service button,
 * used to include/exclude the device,
 * reset it and of course
 * is usable for you.
 * In this example we track the button,
 * once it's pressed we shine the built in LED
 */
 // button pin number
#define BTN_PIN     18
// built in LED number
#define LED_PIN     13

void setup() {
     pinMode(LED_PIN, OUTPUT);        // set LED pin as output
     pinMode(BTN_PIN, INPUT_PULLUP);  // set button pin as Input
}

void loop() {
  // sample button state
  byte buttonState = digitalRead(BTN_PIN);
   
  if (buttonState == HIGH) {      // if button is not pressed
    digitalWrite(LED_PIN, LOW);  // turn the LED off
  } else {                       // if button is pressed
    digitalWrite(LED_PIN, HIGH); // turn the LED on
  }
}

