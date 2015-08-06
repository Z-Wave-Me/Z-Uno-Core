/*
 * That is a Simple Blink Sketch. It just blinks the LED
 */
 
// LED pin number
#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);      // set LED pin as output
}

void loop() {
  digitalWrite(LED_PIN, HIGH);   // turn LED on
  delay(1000);                   // wait for 1 second
  digitalWrite(LED_PIN, LOW);    // turn LED off
  delay(1000);                   // wait for 1 second
}
