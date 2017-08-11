/*
 * Demo sketch for NeoPxel library for Z-Uno
 * (c) Z-Wave.Me 2017 for Z-Uno project
 * some ideas was used from Adafruit library 
 */
 
// LED pin number
#define LED_PIN 13

#include <ZUNO_NeoPixel.h>

#define MAX_PIXELS 14  // Number of pixel in strip or module that we use
                       // ! The maximum supported number is 25
#define PIXEL_SIZE 3   // Number of color components. Do not modify. Must be 3
#define BUFF_SIZE     (MAX_PIXELS * PIXEL_SIZE)

byte pixel_buff[BUFF_SIZE]; // Pixel buffer

// Make object to control neo-pixel module/strip
NeoPixel pixels(pixel_buff, BUFF_SIZE);


void setup() {
  pixels.begin();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  byte i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< MAX_PIXELS; i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / MAX_PIXELS) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
void loop() {
  // Makes a little rainbow ;-)
  rainbowCycle(5);
  delay(10); // wait a little

}
