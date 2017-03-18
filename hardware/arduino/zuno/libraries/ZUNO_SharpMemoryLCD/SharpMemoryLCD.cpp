/*****************************************************************************
* Z-Uno Library for Sharp Memory LCD Displays (e.g Adafruit 1.3" 96x96 display)
******************************************************************************
* SharpMemoryLCD.h - C++ implementation file of the LCD display library
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

#include <SPI.h>
#include "SharpMemoryLCD.h"

// Font pattern definitions copied from:
// http://fontstruct.com/fontstructions/show/310233/5x8_lcd_hd44780u_a02
// The patterns are stored inside the code memory space. Limitations of the
// Z-Uno compiler requires a very strict syntax to use the code space!

const char *Font5x8 = 
"\x00\x01\x01\x01\x00\
\x00\x00\x9E\x00\x00\
\x00\x0E\x01\x0E\x00\
\x28\xFE\x28\xFE\x28\
\x48\x54\xFE\x54\x24\
\x46\x26\x10\xC8\xC4\
\x6C\x92\xAA\x44\xA0\
\x00\x0A\x06\x00\x00\
\x00\x38\x44\x82\x00\
\x00\x82\x44\x38\x00\
\x10\x54\x38\x54\x10\
\x10\x10\x7C\x10\x10\
\x00\xA0\x60\x00\x00\
\x00\x10\x10\x10\x00\
\x00\x00\xC0\x00\x00\
\x40\x20\x10\x08\x04\
\x7C\xA2\x92\x8A\x7C\
\x00\x84\xFE\x80\x00\
\x84\xC2\xA2\x92\x8C\
\x42\x82\x8A\x96\x62\
\x30\x28\x24\xFE\x20\
\x4E\x8A\x8A\x8A\x72\
\x78\x94\x92\x92\x60\
\x02\xE2\x12\x0A\x06\
\x6C\x92\x92\x92\x6C\
\x0C\x92\x92\x52\x3C\
\x00\x6C\x6C\x00\x00\
\x00\xAC\x6C\x00\x00\
\x00\x10\x28\x44\x82\
\x28\x28\x28\x28\x28\
\x82\x44\x28\x10\x00\
\x04\x02\xA2\x12\x0C\
\x64\x92\xF2\x82\x7C\
\xFC\x22\x22\x22\xFC\
\xFE\x92\x92\x92\x6C\
\x7C\x82\x82\x82\x44\
\xFE\x82\x82\x44\x38\
\xFE\x92\x92\x92\x82\
\xFE\x12\x12\x02\x02\
\x7C\x82\x82\xA2\x64\
\xFE\x10\x10\x10\xFE\
\x00\x82\xFE\x82\x00\
\x40\x80\x82\x7E\x02\
\xFE\x10\x28\x44\x82\
\xFE\x80\x80\x80\x80\
\xFE\x04\x08\x04\xFE\
\xFE\x08\x10\x20\xFE\
\x7C\x82\x82\x82\x7C\
\xFE\x12\x12\x12\x0C\
\x7C\x82\xA2\x42\xBC\
\xFE\x12\x32\x52\x8C\
\x8C\x92\x92\x92\x62\
\x02\x02\xFE\x02\x02\
\x7E\x80\x80\x80\x7E\
\x3E\x40\x80\x40\x3E\
\xFE\x40\x30\x40\xFE\
\xC6\x28\x10\x28\xC6\
\x06\x08\xF0\x08\x06\
\xC2\xA2\x92\x8A\x86\
\x00\x00\xFE\x82\x82\
\x04\x08\x10\x20\x40\
\x82\x82\xFE\x00\x00\
\x08\x04\x02\x04\x08\
\x80\x80\x80\x80\x80\
\x00\x02\x04\x08\x00\
\x40\xA8\xA8\xA8\xF0\
\xFE\x90\x88\x88\x70\
\x70\x88\x88\x88\x40\
\x70\x88\x88\x90\xFE\
\x70\xA8\xA8\xA8\x30\
\x10\xFC\x12\x02\x04\
\x10\x28\xA8\xA8\x78\
\xFE\x10\x08\x08\xF0\
\x00\x88\xFA\x80\x00\
\x40\x80\x88\x7A\x00\
\x00\xFE\x20\x50\x88\
\x00\x82\xFE\x80\x00\
\xF8\x08\x30\x08\xF0\
\xF8\x10\x08\x08\xF0\
\x70\x88\x88\x88\x70\
\xF8\x28\x28\x28\x10\
\x10\x28\x28\x30\xF8\
\xF8\x10\x08\x08\x10\
\x90\xA8\xA8\xA8\x40\
\x08\x7E\x88\x80\x40\
\x78\x80\x80\x40\xF8\
\x38\x40\x80\x40\x38\
\x78\x80\x60\x80\x78\
\x88\x50\x20\x50\x88\
\x18\xA0\xA0\xA0\x78\
\x88\xC8\xA8\x98\x88\
\x00\x10\x6C\x82\x00\
\x00\x00\xFE\x00\x00\
\x00\x82\x6C\x10\x00";

// LCD display buffer (limitations of Z-Uno compiler require a definition 
// outside of the class).
static byte SharpMemoryLCD_DisplayBuffer[SMLCD_HEIGHT][SMLCD_WIDTHDIV8];


/******** API - LCD display setup ********/

	
// Constructor - Not really used.
SharpMemoryLCD::SharpMemoryLCD() {
}

// begin - Initializes the SPI interface including CS pin and class variables.
void SharpMemoryLCD::begin() {
	SPI.begin(); 
	pinMode(8, OUTPUT); // Pin 8: CS
	digitalWrite(8, LOW);
	lastUpdateMS=0;
	VBitMask=0;
}


/******** API - Display updates and refreshes ********/


// clearDisplay - Clears the display (but not the display buffer).
void SharpMemoryLCD::clearDisplay() {
	beginTransaction();
	SPI.transfer( 0x04|getVBit() ); // Send command
	SPI.transfer(0); // Send trailer
	endTransaction();
}

// update - Updates the full display (transfers local buffer data to display).
void SharpMemoryLCD::update() {
	beginTransaction();
	SPI.transfer( 0x01|getVBit() ); // Send command
	for(xy_t r=0; r<SMLCD_HEIGHT; r++) { // Send all rows
		SPI.transfer(r+1); // Row -> 1st row=1
		//SPI.transfer(SharpMemoryLCD_DisplayBuffer[r],SMLCD_WIDTHDIV8);
		// Transfer all bytes of a column. The multi-byte transfer command has issues!
		for(xy_t b=0; b<SMLCD_WIDTHDIV8; b++)
			SPI.transfer(SharpMemoryLCD_DisplayBuffer[r][b]);
		SPI.transfer(0); // Send trailer
	}

	SPI.transfer(0); // Send trailer
	endTransaction();
}

// refresh - Refreshes the display when it's time (sends a toggle VCOM command).
void SharpMemoryLCD::refresh() {
	// Check if the VBit polarity has change. Quit the method if not the case.
	char OldVBitMask=VBitMask;
	char NewVBitMask=getVBit();
	if (NewVBitMask==OldVBitMask)
		return;

	// Send a toggle VCOM command
	beginTransaction();
	SPI.transfer( 0x00|NewVBitMask ); // Send command (Toggle VCOM)
	SPI.transfer(0); // Send trailer
	endTransaction();
}


/******** API - Display buffer updates: Clear, line, rect, circle ********/

	
// clear - Clears the display buffer.
void SharpMemoryLCD::clear() {
	for(xy_t r=0; r<SMLCD_HEIGHT; r++)
		for(xy_t b=0; b<SMLCD_WIDTHDIV8; b++)
			SharpMemoryLCD_DisplayBuffer[r][b]=0xff;
}

// setPixel - Sets a single pixel in the display buffer.
void SharpMemoryLCD::setPixel(xy_t x,xy_t y) {
	// Quit the methode if either x or y are outside of the display area
	if (x>=SMLCD_WIDTH || y>=SMLCD_HEIGHT)
		return;
	// Set the bit in function of the display orientation
	#if SMLCD_ORIENTATION==0
		byte Mask=(1<<((SMLCD_HEIGHT-1-x)&0x07));
		SharpMemoryLCD_DisplayBuffer[(SMLCD_WIDTH-1-y)][(SMLCD_HEIGHT-1-x)>>3]&=(~Mask);
	#elif SMLCD_ORIENTATION==90
		byte Mask=(1<<((SMLCD_WIDTH-1-y)&0x07));
		SharpMemoryLCD_DisplayBuffer[x][(SMLCD_WIDTH-1-y)>>3]&=(~Mask);
	#elif SMLCD_ORIENTATION==180
		byte Mask=(1<<(x&0x07));
		SharpMemoryLCD_DisplayBuffer[y][x>>3]&=(~Mask);
	#elif SMLCD_ORIENTATION==270
		byte Mask=(1<<(y&0x07));
		SharpMemoryLCD_DisplayBuffer[(SMLCD_HEIGHT-1-x)][y>>3]&=(~Mask);
	#endif
}

// line - Draws a line between the coordinates (x0,y0) and (x1,y1).
// Line function based on the Bresenham algorithm, copied from 
// https://de.wikipedia.org/wiki/Bresenham-Algorithmus
void SharpMemoryLCD::line(xy_t x0, xy_t y0, xy_t x1, xy_t y1)
{
	int dx =	abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */

	while(1){
		setPixel(x0,y0);
		if (x0==x1 && y0==y1) break;
		e2 = 2*err;
		if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

// rect - Draws a rectangle with the opposite corners (x0,y0) and (x1,y1).
void SharpMemoryLCD::rect (xy_t x0, xy_t y0, xy_t x1, xy_t y1) {
	line(x0,y0,x1,y0);
	line(x0,y0,x0,y1);
	line(x1,y1,x1,y0);
	line(x1,y1,x0,y1);
}


// circle - Draws a circle with center in (x,y). Copied from 
// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void SharpMemoryLCD::circle(xy_t x, xy_t y, xy_t radius)
{
	int dx = radius;
	int dy = 0;
	int err = 0;

	while (dx >= dy) {
		setPixel(x+dx, y+dy);
		setPixel(x+dy, y+dx);
		if(x>dy)
			setPixel(x-dy, y+dx);
		if(x>dx)
			setPixel(x-dx, y+dy);
		if(x>dx&&y>dy)
			setPixel(x-dx, y-dy);
		if(x>dy&&y>dx)
			setPixel(x-dy, y-dx);
		if(y>dx)
			setPixel(x+dy, y-dx);
		if(y>dy)
			setPixel(x+dx, y-dy);

		if (err <= 0)
		{
			dy += 1;
			err += 2*dy + 1;
		}
		if (err > 0)
		{
			dx -= 1;
			err -= 2*dx + 1;
		}
	}
}


/******** API - Display buffer updates: Write methods ********/


// writeChrS - Writes a small character (7 pixel height) into the display 
// buffer. The method uses directly the defined font patterns (Font5x8) without
// scaling them.
xy_t SharpMemoryLCD::writeChrS(xy_t x,xy_t y,char Chr,byte Options) {
	// Ignore non printable characters
	if (Chr<0x20 || Chr>=0x7D)
		return x;
	// Define a font table pointer
	const char *FontPtr=&Font5x8[(Chr-0x20)*5];
	// Loop over each font column and row, set a pixel for each bit set in the 
	// font pattern.
	for (byte xp=0; xp<5; xp++) {
		unsigned char v=*FontPtr++; // Get the font column pattern
		if (!v && (Options&SMLCD_WRITE_TIGHT))
			continue; // Skip the row if it is empty and tight writing enabled
		if (!(Options&SMLCD_WRITE_CHECKONLY)) {
			v>>=1; // ignore the first bit (used to enforce space if tight writing is enabled)
			// Loop over each dot in the column
			for (xy_t yp=y; v; yp++) {
				if (v&1)
					setPixel(x,yp);
				v>>=1; // Go to the next bit in the pattern
			}
		}
		x++;
	}
	return x+1; // return the next character position
}

// writeS - Writes a character string in small character (7 pixel height) into 
// the buffer.
xy_t SharpMemoryLCD::writeS(xy_t x,xy_t y,char *Text,byte Options) {
	while(*Text!=0)
		x=writeChrS(x,y,*Text++,Options);
	return x;
}

// writeS - Writes an integer in small character (11 pixel height) into the display buffer.
xy_t SharpMemoryLCD::writeS(xy_t x,xy_t y,int Val,byte Options) {
	return writeS(x,y,int2Str(Val,Options>>4),Options);
}

// writeChrM - Writes a medium character (11 pixel height) into the display 
// buffer. The method scales the defined font patterns (Font5x8) by 150%, using
// the following interpolation algorithm:
// A 2x2 point window is moved over the 5x8 point font pattern with a 2 point 
// step size. This 2x2 point window is expanded into a 3x3 point window that 
// moves with a 3 step size over the display area. The translation from the 2x2 
// window to the 3x3 window in made in following way:
//                 +--+--+--+--+
//   +--+--+       |a1|b1 b1|c1|   The 2x2 point window takes the following
//   |a1|b1|       +--+--+--+--+   positions in the horizontal direction:
//   +--+--+  ==>  |a2|b2|b2|c2|      0-1, 2-3, 4  
//   |a2|b2|       +--+--+--+--+   And the following ones in the vertical 
//   +--+--+       |a2|b2|b2|c2|   direction (point 0 and 8 are ignored):
//                 +--+--+--+--+      0-1, 2-3, 4-5, 6-7
xy_t SharpMemoryLCD::writeChrM(xy_t x,xy_t y,char Chr,byte Options) {
	// Ignore non printable characters
	if (Chr<0x20 || Chr>=0x7D)
		return x;
	// Define a font table pointer
	const char *FontPtr=&Font5x8[(Chr-0x20)*5];
	// Loop over each font column and row, set a pixel for each bit set in the 
	// font pattern.
	y--;
	for (byte xp=0; xp<3; xp++) {
		// Get the font column pattern for the 3 consecutive columns. In the next 
		// iteration the last column (v2) becomes the first one (v0).
		unsigned char v0=*FontPtr++;
		unsigned char v1=*FontPtr++;
		unsigned char Inc=3;
		if (xp==2) {
			Inc=1;
			v1=0;
		}
		if (Options&SMLCD_WRITE_TIGHT) {
			if (!v0) { // Skip the 1st row if it is empty and tight writing enabled
				x--;
				if (!v1 && xp!=2)
					x-=2;
			} else if (!v1 && xp!=2) {
				Inc=1;
			}
		}

		if (!(Options&SMLCD_WRITE_CHECKONLY)) {
			// ignore the first bit (used to enforce space if tight writing is enabled)
			v0&=0xfe;
			v1&=0xfe;

			// Loop over each dot in the column
			for (xy_t yp=y; v0|v1; yp+=3) {
				if (v0&0x1) {
					setPixel(x,yp); }
				if (v0&0x2) {
					setPixel(x,yp+1);
					setPixel(x,yp+2); }
				if (v0&0x4) {
					setPixel(x,yp+3); }
				if (v1&0x1) {
					setPixel(x+1,yp);
					setPixel(x+2,yp); }
				if (v1&0x2) {
					setPixel(x+1,yp+1);
					setPixel(x+1,yp+2);
					setPixel(x+2,yp+1);
					setPixel(x+2,yp+2); }
				if (v1&0x4) {
					setPixel(x+1,yp+3);
					setPixel(x+2,yp+3); }
				// Go to the next bits in the patterns
				v0>>=2;
				v1>>=2;
			}
		}
		x+=Inc;
	}
	return x+2; // return the next character position
}

// writeM - Writes a character string in medium character (11 pixel height) into the 
xy_t SharpMemoryLCD::writeM(xy_t x,xy_t y,char *Text,byte Options) {
	while(*Text!=0)
		x=writeChrM(x,y,*Text++,Options);
	return x;
}

// writeM - Writes an integer in medium character (11 pixel height) into the display 
xy_t SharpMemoryLCD::writeM(xy_t x,xy_t y,int Val,byte Options) {
	return writeM(x,y,int2Str(Val,Options>>4),Options);
}

// writeChrL - Writes a large character (14 pixel height) into the display 
// buffer. The method scales the defined font patterns (Font5x8) by 200% (each
// defined point is expanded into 2 points in each direction).
xy_t SharpMemoryLCD::writeChrL(xy_t x,xy_t y,char Chr,byte Options) {
	// Ignore non printable characters
	if (Chr<0x20 || Chr>=0x7D)
		return x;
	// Define a font table pointer
	const char *FontPtr=&Font5x8[(Chr-0x20)*5];
	// Loop over each font column and row, set 2x2 pixels for each bit set in 
	// the font pattern.
	for (byte xp=0; xp<5; xp++) {
		unsigned char v=*FontPtr++; // Get the font column pattern
		if (!v && (Options&SMLCD_WRITE_TIGHT))
			continue; // Skip the row if it is empty and tight writing enabled
		if (!(Options&SMLCD_WRITE_CHECKONLY)) {
			v>>=1; // ignore the first bit (used to enforce space if tight writing is enabled)
			for (xy_t yp=y; v; yp+=2) {
				if (v&1) {
					setPixel(x,yp);
					setPixel(x,yp+1);
					setPixel(x+1,yp);
					setPixel(x+1,yp+1);
				}
				v>>=1; // Go to the next bit in the pattern
			}
		}
		x+=2;
	}
	return x+2; // return the next character position
}

// writeL - Writes a character string in large character (14 pixel height) into the 
xy_t SharpMemoryLCD::writeL(xy_t x,xy_t y,char *Text,byte Options) {
	while(*Text!=0)
		x=writeChrL(x,y,*Text++,Options);
	return x;
}

// writeL - Writes an integer in large character (14 pixel height) into the display 
xy_t SharpMemoryLCD::writeL(xy_t x,xy_t y,int Val,byte Options) {
	return writeL(x,y,int2Str(Val,Options>>4),Options);
}

// writeFit - Writes a character string into the display buffer using the biggest
xy_t SharpMemoryLCD::writeFit(xy_t x0,xy_t y0, xy_t x1,char *Text,byte Options) {
	byte CenterY=((Options&SMLCD_WRITE_CENTERY)!=0);
	byte OptionsCheckOnly=(Options&SMLCD_WRITE_TIGHT)|2;

	xy_t x=writeL(x0,y0,Text,OptionsCheckOnly);
	if (x<=x1) {
		x=writeL(x0,y0,Text,Options);
		return x;
	}

	x=writeM(x0,y0,Text,OptionsCheckOnly);
	if (x<=x1) {
		x=writeM(x0,y0+2*CenterY,Text,Options);
		return x;
	}

	x=writeS(x0,y0+4*CenterY,Text,Options);
	return x;
}

// writeFit - Writes an integer into the display buffer using the biggest possible font
xy_t SharpMemoryLCD::writeFit(xy_t x0,xy_t y0, xy_t x1,int Val,byte Options) {
	return writeFit(x0,y0,x1,int2Str(Val,Options>>4),Options);
}


/* Internal methods */

// beginTransaction - Begins a SPI communication frame.
void SharpMemoryLCD::beginTransaction() {
	SPISettings spi_settings = SPISettings(1000000, LSBFIRST, SPI_MODE0);
	SPI.beginTransaction(&spi_settings);
	digitalWrite(8, HIGH);
}

// endTransaction - Completes a SPI communication frame.
void SharpMemoryLCD::endTransaction() {
	SPI.endTransaction();
	digitalWrite(8, LOW);
}

// VCOM toggling bit should change polarity every second. The getVBit method 
// provides the VCOM bit in function of the time.
char SharpMemoryLCD::getVBit() {
	unsigned long Now=millis();
	if(Now-lastUpdateMS>=750) {
		VBitMask^=0x02;
		lastUpdateMS=Now;
	}
	return VBitMask;
}

// Function: int2Str - Converts a integer into a string. This function hasn't 
// been added to the class SharpMemoryLCD to reduce stack usage.
char *int2Str(int Val, byte DecimalPos) {
	// NegDecPos - Bit 7: Neg, Bits 3..0: Decimal position (stack usage optimized)
	byte NegDecPos=DecimalPos;
	if (NegDecPos==0)
		NegDecPos=0x7f;
	NegDecPos|=0x40; // Avoids underflow
	if(Val<0) {
		NegDecPos|=0x80;
		Val=-Val;
	}
	static char Str[10];
	char *p=&Str[10];
	*--p=0;
	
	while(1) {
		if (!(NegDecPos&0x3f))
			*--p='.';
		NegDecPos--;
		*--p=(char)('0'+(Val%10));
		Val/=10;
		if (Val==0)
			break;
	}
	if (NegDecPos&0x80) {
		*--p='-';
	}
	return p;
}
