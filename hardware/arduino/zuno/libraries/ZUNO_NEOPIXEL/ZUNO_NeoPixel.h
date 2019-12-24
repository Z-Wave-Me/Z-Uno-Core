#pragma once

/*
 * Copyright (c) 2017 by Z-WAVE>ME (www.z-wave.me) for Z-Uno Project (z-uno.z-wave.me)
 * 		
 *						Small NeoPixel library
 *
 * 			You can control up to 25 neopixels with that from Z-Unp
 * 
 *  
 *     OUTPUT 		PIN#2 (Have to be inverted before connected to neopixel D0).
 *     For example you can use 2n7000 n-channel MOSFET to do that like this:
 *           
 *            GND (SOURCE)
 *                          \  
 *				             + -
 *              PIN#2(GATE) -+ | )  2n7000
 *                           + -
 *                          / 
 *     NEOPixel D0(DRAIN)--*--[ 1K ]--|> 3V3
 *
 * 
 *     Warning:		Library uses hardware SPI, so PIN#0, PIN#1 can't be used in this case for GPIO
 *								
 */

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

class NeoPixel
{
	public:

		NeoPixel(byte * pixelbuff, word count):
										pixel_buffer(pixelbuff),
										pixel_buffer_size(count)
										{ brightness = 255; };

	public:
		void begin();
		void clear();
		void setPixelWhite(byte index, byte brightness);
		void setPixelColor(byte index, byte r, byte g, byte b);
		void setPixelColor(byte index, uint32_t color);
		void show();
		void end();
		void setBrightness(byte b);
		byte getBrightness();
		uint32_t Color(byte r, byte g, byte b);


	private:

		byte * pixel_buffer;
		word   pixel_buffer_size;
		byte   brightness;




};