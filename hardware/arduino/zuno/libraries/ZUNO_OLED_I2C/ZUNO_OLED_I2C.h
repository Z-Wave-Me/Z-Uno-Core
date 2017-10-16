
/*
  OLED_I2C.h - Arduino/chipKit library support for 128x64 pixel SSD1306 OLEDs
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library has been made to make it easy to use 128x64 pixel OLED displays
  based on the SSD1306 controller chip with an Arduino or a chipKit.

  You can always find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.

  Adoptated for Z-Uno project by Z-Wave>ME 2016
  Reworked to reduce stack and memory usage by  Z-Wave>ME 2017
*/

#ifndef OLED_I2C_h
#define OLED_I2C_h


#define FONT_SYMBOLW_OFFSET     0
#define FONT_SYMBOLH_OFFSET     1
#define FONT_STARTSYMBOL_OFFSET 2
#define FONT_DATA_OFFSET        3



#include "Arduino.h"

// ! Notice y-coord always defines in rows (8-pixels clocks)
// It can be in interval [0;7]
// It's a hardware feature. We can't use 1k of memory 
// for inner software frame-buffer, so we use it "as is" 
// x-coord we define in pixels   
class OLED:public Print
{
	public:
		OLED();
		void	begin();
		void	setBrightness(uint8_t value);
    // Writes bitmap contains in pdata from current position
		void 	writeData(char * pdata);
		void 	invert(bool mode);
		void  clrscr();
		void 	gotoXY(byte x, byte y);
		void  off();
		void  on();
    void  setFont(char * font);
    void  fillRect(byte w, byte h, byte fill);
		virtual void write(uint8_t value);
    
	protected:
		byte 	cx,cy;
    byte  symbol_w,symbol_h;
    byte  start_symbol;  
		byte * curr_font;


};

extern char * SmallFont;

#endif