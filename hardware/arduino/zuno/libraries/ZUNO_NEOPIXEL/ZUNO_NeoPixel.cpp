
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

#include "ZUNO_NeoPixel.h"
#include "ZUNO_Channels.h"
#include "SPI.h"
#include "ArduinoCAPI.h"
#include <Arduino.h>

extern byte * dma_buff;	
extern byte   dma_buff_size;
extern word   np_tmp;

// SPI freq = 4MHz !!!
SPISettings spi_settings(4000000, MSBFIRST, SPI_MODE0);
byte g_neo_i;

void fillNeoByte(byte value); 
void NeoPixel::begin()
{
	dma_buff = (BYTE *) 3064; // DMA Adress of IR. Do not modify!
	
	// We use SPI, setting it up
	SPI.begin();
  	//SPI.beginTransaction(&spi_settings);
	clear();
	//SPI.endTransaction();

}
void NeoPixel::clear()
{
	/*word i;

	for(i=0;i<pixel_buffer_size;i++)
		pixel_buffer[i] = 0;*/
	dma_buff_size = pixel_buffer_size;
	dma_buff_size <<= 2;
	memset(pixel_buffer, 0, pixel_buffer_size);
	memset(dma_buff, 0x77, dma_buff_size);
	SPI.beginTransaction(&spi_settings);
	zunoSysCall(ZUNO_FUNC_SPI0_TXDMA, dma_buff_size, byte(0));
	delay(2);
	SPI.endTransaction();
	
}
void NeoPixel::setPixelColor(byte index, byte r, byte g, byte b)
{
	index <<= 2;
	pixel_buffer[index] 	= g;
	pixel_buffer[index+1] 	= r;
	pixel_buffer[index+2] 	= b;

}
void NeoPixel::setPixelWhite(byte index, byte brightness){
	index <<= 2;
	pixel_buffer[index+3] 	= brightness;
}
void NeoPixel::setPixelColor(byte index, uint32_t color)
{
	index <<= 2;
	pixel_buffer[index] 	= (color >> 16) & 0xFF;
	pixel_buffer[index+1] 	= (color >> 8) & 0xFF;
	pixel_buffer[index+2] 	= color & 0xFF;
	//pixel_buffer[i+3] 	= 0;
}
uint32_t  NeoPixel::Color(byte r, byte g, byte b)
{
	return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;

}
void fillNeoByte(byte value){
	byte i = 4;
	while(i){
		switch(value & 0xC0){
				case 0x00:
					dma_buff[dma_buff_size] = 0x77;
					break;
				case 0x40:
					dma_buff[dma_buff_size] = 0x71;
					break;
				case 0x80:
					dma_buff[dma_buff_size] = 0x17;
					break;
				case 0xC0:
					dma_buff[dma_buff_size] = 0x11;
					break;
		}
		dma_buff_size++;
		value <<= 2;
		i--;
	}	
}
void NeoPixel::show()
{
	byte i;
	dma_buff_size = 0;
	for(i=0;i<pixel_buffer_size;i++)
	{
		np_tmp  = pixel_buffer[i];
		np_tmp  *= brightness;
		np_tmp  >>= 8;
		fillNeoByte(np_tmp);
	}
	SPI.beginTransaction(&spi_settings);
	zunoSysCall(ZUNO_FUNC_SPI0_TXDMA, dma_buff_size, byte(0));
	delay(2);
	SPI.endTransaction();
    
}
void NeoPixel::setBrightness(byte b)
{
	brightness = b; 
}
byte NeoPixel::getBrightness()
{
	return brightness;
}
void NeoPixel::end()
{
	SPI.end();
}
word   np_tmp;
byte * dma_buff;
byte   dma_buff_size;