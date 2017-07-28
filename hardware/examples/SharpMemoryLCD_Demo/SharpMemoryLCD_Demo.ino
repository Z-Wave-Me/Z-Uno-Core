/*****************************************************************************
* Z-Uno Library for Sharp Memory LCD Displays (e.g Adafruit 1.3" 96x96 display)
******************************************************************************
* SharpMemoryLCD_Demo.ino - Z-Uno demo application for the LDC display library
* 
* This Z-Uno application demonstrates the capabilities of the Sharp Memory LCD
* Display Library.
*
* Copyright (C) 2017 Andreas Drollinger
*****************************************************************************
* See the file "LICENSE" for information on usage and redistribution
* of this file, and for a DISCLAIMER OF ALL WARRANTIES.
\****************************************************************************/

#include "SharpMemoryLCD.h"
SharpMemoryLCD LCD=SharpMemoryLCD();

void setup() {
	LCD.begin();
}

// Demo write with small characters
void DemoWriteS() {
	LCD.clear();
	
	LCD.writeS(16,0,"Small Chars",SMLCD_WRITE_TIGHT);

	xy_t x=0, y=18;
	for (byte c=32; c<128; c++) {
		x=LCD.writeChrS(x,y,c);
		if (x>96-5) {
			x=0;
			y+=9;
		}
	}

	LCD.update();
}

// Demo write with medium characters
void DemoWriteM() {
	LCD.clear();
	
	LCD.writeS(16,0,"Medium Chars",SMLCD_WRITE_TIGHT);

	xy_t x=0, y=15;
	for (byte c=33; c<128; c++) {
		x=LCD.writeChrM(x,y,c,SMLCD_WRITE_TIGHT);
		if (x>96-7) {
			x=0;
			y+=14;
		}
	}

	LCD.update();
}

// Demo write with large characters
void DemoWriteL() {
	LCD.clear();
	
	LCD.writeS(16,0,"Large Chars",SMLCD_WRITE_TIGHT);

	xy_t x=0, y=15;
	for (byte c=33; c<128; c++) {
		x=LCD.writeChrL(x,y,c,SMLCD_WRITE_TIGHT);
		if (x>96-10) {
			x=0;
			y+=18;
		}
	}

	LCD.update();
}

// Demo of the writeFit method
void DemoWriteFit() {
	LCD.clear();

	LCD.writeS(16,0,"Write Fit",SMLCD_WRITE_TIGHT);

	LCD.rect(0,8,95,49);
	LCD.writeFit(2,10,95,"8 chars",SMLCD_WRITE_TIGHT);
	LCD.writeFit(2,27,95,"More chars",SMLCD_WRITE_TIGHT);
	LCD.writeFit(2,41,95,"Even more chars",SMLCD_WRITE_TIGHT);

	LCD.rect(35,49,65,95);
	LCD.writeFit(37,51,63,"1.0",SMLCD_WRITE_TIGHT);
	LCD.writeFit(37,69,63,"-1.0",SMLCD_WRITE_TIGHT);
	LCD.writeFit(37,87,63,"128.2",SMLCD_WRITE_TIGHT);
	
	LCD.update();
}

// Demo of the drawing methods
void DemoDrawings() {
	LCD.clear();

	LCD.writeS(26,0,"Drawings",SMLCD_WRITE_TIGHT);
	
	for (int k=0; k<9; k++)
		LCD.rect(40-5*k, 48-5*k, 55+5*k, 55+5*k);

	for (int k=0; k<4; k++) {
		LCD.circle( 0,  0, 23-5*k);
		LCD.circle( 0, 95, 23-5*k);
		LCD.circle(95,  0, 23-5*k);
		LCD.circle(95, 95, 23-5*k);
	}

	for (int k=0; k<12; k++)
		LCD.line(25+4*k, 70+k, 47, 95);

	LCD.update();
}

// Wait a specified number of seconds, refresh the display every second
void Wait(byte Seconds) {
	while(Seconds>0) {
		delay(1000);
		LCD.refresh();
		Seconds--;
	}
}

// Iterate through all the different demos
void loop() {
	DemoWriteS();
	Wait(4);
	DemoWriteM();
	Wait(4);
	DemoWriteL();
	Wait(4);
	DemoWriteFit();
	Wait(4);
	DemoDrawings();
	Wait(4);
}
