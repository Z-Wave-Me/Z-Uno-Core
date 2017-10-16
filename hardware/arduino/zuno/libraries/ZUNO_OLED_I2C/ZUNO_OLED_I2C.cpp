/*
  OLED_I2C.cpp - Arduino/chipKit library support for 128x64 pixel SSD1306 OLEDs
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
*/

#include "ZUNO_OLED_I2C.h"
#include "Wire.h"

#define SSD1306_ADDR		0x3C

#define LEFT	0
#define RIGHT	9999
#define CENTER	9998

#define SSD1306_COMMAND			0x00
#define SSD1306_DATA			0xC0
#define SSD1306_DATA_CONTINUE	0x40

#define RST_NOT_IN_USE	255

// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define SSD1306_SET_CONTRAST_CONTROL					0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME					0xA4
#define SSD1306_DISPLAY_ALL_ON							0xA5
#define SSD1306_NORMAL_DISPLAY							0xA6
#define SSD1306_INVERT_DISPLAY							0xA7
#define SSD1306_DISPLAY_OFF								0xAE
#define SSD1306_DISPLAY_ON								0xAF
#define SSD1306_NOP										0xE3
// Scrolling Commands
#define SSD1306_HORIZONTAL_SCROLL_RIGHT					0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT					0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT	0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT		0x2A
#define SSD1306_DEACTIVATE_SCROLL						0x2E
#define SSD1306_ACTIVATE_SCROLL							0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA				0xA3
// Addressing Setting Commands
#define SSD1306_SET_LOWER_COLUMN						0x00
#define SSD1306_SET_HIGHER_COLUMN						0x10
#define SSD1306_MEMORY_ADDR_MODE						0x20
#define SSD1306_SET_COLUMN_ADDR							0x21
#define SSD1306_SET_PAGE_ADDR							0x22
// Hardware Configuration Commands
#define SSD1306_SET_START_LINE							0x40
#define SSD1306_SET_SEGMENT_REMAP						0xA0
#define SSD1306_SET_MULTIPLEX_RATIO						0xA8
#define SSD1306_COM_SCAN_DIR_INC						0xC0
#define SSD1306_COM_SCAN_DIR_DEC						0xC8
#define SSD1306_SET_DISPLAY_OFFSET						0xD3
#define SSD1306_SET_COM_PINS							0xDA
#define SSD1306_CHARGE_PUMP								0x8D
// Timing & Driving Scheme Setting Commands
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO				0xD5
#define SSD1306_SET_PRECHARGE_PERIOD					0xD9
#define SSD1306_SET_VCOM_DESELECT						0xDB


// Macroses to make code clean
// ----------------------------------------------------------------------------------------------------------------
#define _sendTWIcommand(D) g_oled_command = D; OLED_writeCommand();
#define _sendTWIAddr(a1,a2,a3,a4) g_oled_addr1=a1;g_oled_addr2=a2;g_oled_addr3=a3;g_oled_addr4=a4;OLED_SetAdress();
//-----------------------------------------------------------------------------------------------------------------
extern byte g_oled_command;
extern word g_oled_count;
extern byte g_oled_cb;
extern byte g_oled_addr1;
extern byte g_oled_addr2;
extern byte g_oled_addr3;
extern byte g_oled_addr4;
// -----------------------------------------------------
// GLOBAL variables
byte g_oled_command = 0;
byte g_oled_addr1 = 0;
byte g_oled_addr2 = 0;
byte g_oled_addr3 = 0;
byte g_oled_addr4 = 0;
byte g_oled_cb;
word g_oled_count;
// -----------------------------------------------------
// -----------------------------------------------------
// Auxilary functions to reduce stack & memory usage
// -----------------------------------------------------
void OLED_writeCommand() {
	Wire.beginTransmission(SSD1306_ADDR);
  	Wire.write(SSD1306_COMMAND);
  	Wire.write(g_oled_command);
  	Wire.endTransmission();	
}
void OLED_SetAdress() {
	_sendTWIcommand(SSD1306_SET_COLUMN_ADDR);
	_sendTWIcommand(g_oled_addr1);
	_sendTWIcommand(g_oled_addr2);
	_sendTWIcommand(SSD1306_SET_PAGE_ADDR);
	_sendTWIcommand(g_oled_addr3);
	_sendTWIcommand(g_oled_addr4);
}
// -----------------------------------------------------
OLED::OLED(){ 
	setFont(SmallFont);
}
void OLED::begin()
{
	

	Wire.begin();

	_sendTWIcommand(SSD1306_DISPLAY_OFF);
    _sendTWIcommand(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    _sendTWIcommand(0x80);
    _sendTWIcommand(SSD1306_SET_MULTIPLEX_RATIO);
    _sendTWIcommand(0x3F);
    _sendTWIcommand(SSD1306_SET_DISPLAY_OFFSET);
    _sendTWIcommand(0x0);
    _sendTWIcommand(SSD1306_SET_START_LINE | 0x0);
    _sendTWIcommand(SSD1306_CHARGE_PUMP);
	_sendTWIcommand(0x14);
    _sendTWIcommand(SSD1306_MEMORY_ADDR_MODE);
    _sendTWIcommand(0x00);
    _sendTWIcommand(SSD1306_SET_SEGMENT_REMAP | 0x1);
    _sendTWIcommand(SSD1306_COM_SCAN_DIR_DEC);
    _sendTWIcommand(SSD1306_SET_COM_PINS);
    _sendTWIcommand(0x12);
    _sendTWIcommand(SSD1306_SET_CONTRAST_CONTROL);
	_sendTWIcommand(0xCF);
    _sendTWIcommand(SSD1306_SET_PRECHARGE_PERIOD);
	_sendTWIcommand(0xF1);
    _sendTWIcommand(SSD1306_SET_VCOM_DESELECT);
    _sendTWIcommand(0x40);
    _sendTWIcommand(SSD1306_DISPLAY_ALL_ON_RESUME);
    _sendTWIcommand(SSD1306_NORMAL_DISPLAY);
	_sendTWIcommand(SSD1306_DISPLAY_ON);

	cx = 0; 
	cy = 0;

}
void OLED::clrscr()
{
	_sendTWIAddr(0,127,0,7);
	Wire.beginTransmission(SSD1306_ADDR);
  	Wire.write(SSD1306_DATA_CONTINUE);
  	g_oled_count = 1024;
  	while(g_oled_count--)
		Wire.write(0x00);
	Wire.endTransmission();	


}
// It's a bad idea to take there a lot of variables
// Just pass pointer to structure like this
// struct OLED_SPRITE_IMAGE { byte w; byte h; byte data[MAX_DATA]};
// NOTICE: structure uses a native format of the display 
// 		   w has pixel units,
// 		   h has 8-pixel block units
//         data  packed by columns. Each byte contains 8 verticals pixels of current column
void OLED::writeData(char * pdata) {
	_sendTWIAddr(cx,cx+pdata[0]-1,cy,cy+pdata[1]-1);
	// Write data consiquently
	Wire.beginTransmission(SSD1306_ADDR);
  	Wire.write(SSD1306_DATA_CONTINUE);
  	g_oled_count = pdata[0]*pdata[1];
  	// Shift to the beginning of the image data
  	pdata += 2;
  	// Go->>>
  	while(g_oled_count--) {
		Wire.write(*pdata);
		pdata++;
	}
	Wire.endTransmission();	
}


void  OLED::setFont(char * font) {
	if(font == 0) // Default
		font = SmallFont;
	symbol_w 		= font[FONT_SYMBOLW_OFFSET];
	symbol_h 		= font[FONT_SYMBOLH_OFFSET];
	start_symbol	= font[FONT_STARTSYMBOL_OFFSET];
	curr_font       = (byte*)(font + FONT_DATA_OFFSET);
}
void  OLED::fillRect(byte w, byte h, byte fill)
{
	g_oled_count = w*h;
	if(fill)
		fill = 0xFF;
	_sendTWIAddr(cx,cx+w-1,cy,cy+h-1);
	// Send data to display consiquently
	Wire.beginTransmission(SSD1306_ADDR);
  	Wire.write(SSD1306_DATA_CONTINUE);
  	while(g_oled_cb--) {
  	     Wire.write(fill);
		g_oled_count++;
	}
	Wire.endTransmission();		
}
void OLED::write(uint8_t value) {
	// Process carrage return
	if(value == '\r')
	{	
		// Carrage return
		cx = 0;	
		return;
	}
	// Process NEWLINE
	if(value == '\n') {
		// Next line
		cy = (cy + 1)%8;
		// Carrage return
		cx = 0;	
		return;
	}
	// Calculate size of the symbol in memory
	g_oled_cb = symbol_w*symbol_h;
	// Calculate start "pixel" of the symbol
	g_oled_count = g_oled_cb*(value  - start_symbol);
	// Setting up working area 
	_sendTWIAddr(cx,cx+symbol_w-1,cy,cy+symbol_h-1);
	// Send data to display consiquently
	Wire.beginTransmission(SSD1306_ADDR);
  	Wire.write(SSD1306_DATA_CONTINUE);
  	while(g_oled_cb--) {
		Wire.write(curr_font[g_oled_count]);
		g_oled_count++;
	}
	Wire.endTransmission();	
	// Increase the current position 
	cx += symbol_w;
	// Increase the current line
	if((cx + symbol_w) > 127){
		cx = 0;
		cy = (cy + 1)%8;
	}

}
void OLED::on() {
	_sendTWIcommand(SSD1306_DISPLAY_ON);
}
void OLED::off() {
	_sendTWIcommand(SSD1306_DISPLAY_OFF);
}
void OLED::gotoXY(byte x, byte y) {
	cx = x;
	cy = y;
}
void OLED::setBrightness(uint8_t value) {
	_sendTWIcommand(SSD1306_SET_CONTRAST_CONTROL);
	_sendTWIcommand(value);
}
void OLED::invert(bool mode) {
	if (mode) {
		_sendTWIcommand(SSD1306_INVERT_DISPLAY);
	}
	else {
		_sendTWIcommand(SSD1306_NORMAL_DISPLAY);
	}
}


// Default font data
char * SmallFont = 
"\x06\x01\x20"
"\x00\x00\x00\x00\x00\x00" // space
"\x00\x00\x00\x2f\x00\x00" // !
"\x00\x00\x07\x00\x07\x00" // "
"\x00\x14\x7f\x14\x7f\x14" // #
"\x00\x24\x2a\x7f\x2a\x12"
"\x00\x23\x13\x08\x64\x62"
"\x00\x36\x49\x55\x22\x50"
"\x00\x00\x05\x03\x00\x00"
"\x00\x00\x1c\x22\x41\x00"
"\x00\x00\x41\x22\x1c\x00"
"\x00\x14\x08\x3e\x08\x14"
"\x00\x08\x08\x3e\x08\x08"
"\x00\x00\x00\xa0\x60\x00"
"\x00\x08\x08\x08\x08\x08"
"\x00\x00\x60\x60\x00\x00"
"\x00\x20\x10\x08\x04\x02"
"\x00\x3e\x51\x49\x45\x3e"
"\x00\x00\x42\x7f\x40\x00"
"\x00\x42\x61\x51\x49\x46"
"\x00\x21\x41\x45\x4b\x31"
"\x00\x18\x14\x12\x7f\x10"
"\x00\x27\x45\x45\x45\x39"
"\x00\x3c\x4a\x49\x49\x30"
"\x00\x01\x71\x09\x05\x03"
"\x00\x36\x49\x49\x49\x36"
"\x00\x06\x49\x49\x29\x1e"
"\x00\x00\x36\x36\x00\x00"
"\x00\x00\x56\x36\x00\x00"
"\x00\x08\x14\x22\x41\x00"
"\x00\x14\x14\x14\x14\x14"
"\x00\x00\x41\x22\x14\x08"
"\x00\x02\x01\x51\x09\x06"
"\x00\x32\x49\x59\x51\x3e"
"\x00\x7c\x12\x11\x12\x7c"
"\x00\x7f\x49\x49\x49\x36"
"\x00\x3e\x41\x41\x41\x22"
"\x00\x7f\x41\x41\x22\x1c"
"\x00\x7f\x49\x49\x49\x41"
"\x00\x7f\x09\x09\x09\x01"
"\x00\x3e\x41\x49\x49\x7a"
"\x00\x7f\x08\x08\x08\x7f"
"\x00\x00\x41\x7f\x41\x00"
"\x00\x20\x40\x41\x3f\x01"
"\x00\x7f\x08\x14\x22\x41"
"\x00\x7f\x40\x40\x40\x40"
"\x00\x7f\x02\x0c\x02\x7f"
"\x00\x7f\x04\x08\x10\x7f"
"\x00\x3e\x41\x41\x41\x3e"
"\x00\x7f\x09\x09\x09\x06"
"\x00\x3e\x41\x51\x21\x5e"
"\x00\x7f\x09\x19\x29\x46"
"\x00\x46\x49\x49\x49\x31"
"\x00\x01\x01\x7f\x01\x01"
"\x00\x3f\x40\x40\x40\x3f"
"\x00\x1f\x20\x40\x20\x1f"
"\x00\x3f\x40\x38\x40\x3f"
"\x00\x63\x14\x08\x14\x63"
"\x00\x07\x08\x70\x08\x07"
"\x00\x61\x51\x49\x45\x43"
"\x00\x00\x7f\x41\x41\x00"
"\xaa\x55\xaa\x55\xaa\x55"
"\x00\x00\x41\x41\x7f\x00"
"\x00\x04\x02\x01\x02\x04"
"\x00\x40\x40\x40\x40\x40"
"\x00\x00\x03\x05\x00\x00"
"\x00\x20\x54\x54\x54\x78"
"\x00\x7f\x48\x44\x44\x38"
"\x00\x38\x44\x44\x44\x20"
"\x00\x38\x44\x44\x48\x7f"
"\x00\x38\x54\x54\x54\x18"
"\x00\x08\x7e\x09\x01\x02"
"\x00\x18\xa4\xa4\xa4\x7c"
"\x00\x7f\x08\x04\x04\x78"
"\x00\x00\x44\x7d\x40\x00"
"\x00\x40\x80\x84\x7d\x00"
"\x00\x7f\x10\x28\x44\x00"
"\x00\x00\x41\x7f\x40\x00"
"\x00\x7c\x04\x18\x04\x78"
"\x00\x7c\x08\x04\x04\x78"
"\x00\x38\x44\x44\x44\x38"
"\x00\xfc\x24\x24\x24\x18"
"\x00\x18\x24\x24\x18\xfc"
"\x00\x7c\x08\x04\x04\x08"
"\x00\x48\x54\x54\x54\x20"
"\x00\x04\x3f\x44\x40\x20"
"\x00\x3c\x40\x40\x20\x7c"
"\x00\x1c\x20\x40\x20\x1c"
"\x00\x3c\x40\x30\x40\x3c"
"\x00\x44\x28\x10\x28\x44"
"\x00\x1c\xa0\xa0\xa0\x7c"
"\x00\x44\x64\x54\x4c\x44"
"\x00\x00\x10\x7c\x82\x00"// {
"\x00\x00\x00\xff\x00\x00"// |
"\x00\x00\x82\x7c\x10\x00"// }
"\x00\x00\x06\x09\x09\x06"; // ~
