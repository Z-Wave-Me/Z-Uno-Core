/*****************************************************************************
* Z-Uno Library for Sharp Memory LCD Displays (e.g Adafruit 1.3" 96x96 display)
******************************************************************************
* SharpMemoryLCD.h - C++ header file of the LCD display library
* 
* Files that are part of this library:
*   - README.md           - Comprehensive library documentation
*   - SharpMemoryLCD.h    - C++ header file
*   - SharpMemoryLCD.cpp  - C++ implementation file
*   - LICENSE             - License information
*
* Copyright (C) 2017 Andreas Drollinger
*****************************************************************************
* See the file "LICENSE" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
\****************************************************************************/

#include <Arduino.h>

// Display size and orientation (in degree), can be adapted to the display and 
// application. Supported orientations are: 0, 90, 180, 270.
#define SMLCD_HEIGHT 96
#define SMLCD_WIDTH 96
#define SMLCD_ORIENTATION 90

// Display width in bytes
#define SMLCD_WIDTHDIV8 SMLCD_WIDTH/8

// Coordinate type and NA (not applicable/defined) coordinate (depends on the 
// screen size)
#if SMLCD_HEIGHT<256 && SMLCD_WIDTH<256
	typedef byte xy_t;
	#define xy_tNA 255
#else
	typedef unsigned int xy_t;
	#define xy_tNA 0xffff
#endif

// Write options
#define SMLCD_WRITE_TIGHT 1
#define SMLCD_WRITE_CHECKONLY 2
#define SMLCD_WRITE_CENTERY 4
#define SMLCD_WRITE_DECIMALPOS(Pos) (Pos<<4)

// Sharp memory LCD display class definition
class SharpMemoryLCD {
	public:

	/******** API - LCD display setup ********/
	
		// Constructor - Not really used.
		SharpMemoryLCD();

		// begin - Initializes the SPI interface including CS pin and class variables.
		void begin();

	/******** API - Display updates and refreshes ********/

		// clearDisplay - Clears the display (but not the display buffer).
		void clearDisplay();

		// update - Updates the full display (transfers local buffer data to display).
		void update();

		// refresh - Refreshes the display when it's time (sends a toggle VCOM command).
		void refresh();

	/******** API - Display buffer updates: Clear, line, rect, circle ********/

		// clear - Clears the display buffer.
		void clear();

		// setPixel - Sets a single pixel.
		void setPixel(xy_t x,xy_t y);

		// line - Draws a line between the coordinates (x0,y0) and (x1,y1).
		void line(xy_t x0, xy_t y0, xy_t x1, xy_t y1);

		// rect - Draws a rectangle with the opposite corners (x0,y0) and (x1,y1).
		void rect (xy_t x0, xy_t y0, xy_t x1, xy_t y1);

		// circle - Draws a circle with center in (x,y).
		void circle(xy_t x0, xy_t y0, xy_t radius);

	/******** API - Display buffer updates: Write methods ********/

		// writeChrS - Writes a small character (7 pixel height) into the display buffer.
		xy_t writeChrS(xy_t x,xy_t y,char Chr,byte Options=0);

		// writeS - Writes a character string in small character (7 pixel height) into 
		// the buffer.
		xy_t writeS(xy_t x,xy_t y,char *Text,byte Options=0);

		// writeS - Writes an integer in small character (11 pixel height) into the display buffer.
		xy_t writeS(xy_t x,xy_t y,int Val,byte Options=0);
		xy_t writeChrM(xy_t x,xy_t y,char Chr,byte Options=0);
		xy_t writeM(xy_t x,xy_t y,char *Text,byte Options=0);
		xy_t writeM(xy_t x,xy_t y,int Val,byte Options=0);
		xy_t writeChrL(xy_t x,xy_t y,char Chr,byte Options=0);
		xy_t writeL(xy_t x,xy_t y,char *Text,byte Options=0);
		xy_t writeL(xy_t x,xy_t y,int Val,byte Options=0);
		xy_t writeFit(xy_t x0,xy_t y0, xy_t x1,char *Text,byte Options=0);
		xy_t writeFit(xy_t x0,xy_t y0, xy_t x1,int Val,byte Options=0);

	/******** Internal methods and class variables ********/
	private:
		// beginTransaction - Begins a SPI communication frame.
		void beginTransaction();

		// endTransaction - Completes a SPI communication frame.
		void endTransaction();
		
		// VCOM toggling bit should change polarity every second. The getVBit 
		// method provides the VCOM bit in function of the time.
		char getVBit();
		
		// Stores the time of the last display update. Used to determine the VCOM
		// bit polarity.
		unsigned long lastUpdateMS;
		
		// Value defined by getVBit() that alternates between 0x00 and 0x02. Used
		// to add the VBit to the different SPI commands for the display.
		char VBitMask;
};

// Function: int2Str - Converts a integer into a string.
char *int2Str(int v, byte DecimalPos=0);

