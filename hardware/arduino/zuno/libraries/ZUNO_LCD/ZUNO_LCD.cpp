#include "ZUNO_LCD.h"
#include <Wire.h>

#define En 0x4  // Enable bit
#define Rw 0x2  // Read/Write bit
#define Rs 0x1  // Register select bit

extern byte g_lcd_bl;
extern byte g_lcd_addr;

CCBYTE SYMB_SHIFT[]={0x00, 0x40, 0x14, 0x54};
void lcd_write4bits(byte b){
  b |= g_lcd_bl;
  Wire.beginTransmission(g_lcd_addr);
  Wire.write(b);
  Wire.endTransmission();
  delayMicroseconds(2);
  Wire.beginTransmission(g_lcd_addr);
  Wire.write(b|En);
  Wire.endTransmission();
  delayMicroseconds(10);   // enable pulse must be >450ns
  Wire.beginTransmission(g_lcd_addr);
  Wire.write(b & ~En);
  Wire.endTransmission();
  delayMicroseconds(50);    // commands need > 37us to settle
}
void lcd_send(byte b, byte c){
    lcd_write4bits((b & 0xF0) | c);
    lcd_write4bits(((b << 4)&0xF0) | c);
}



LCD_I2C::LCD_I2C(byte cols,byte rows,byte addr):_cols(cols),_rows(rows),_addr(addr),_cx(0),_cy(0),_bl(LCD_BACKLIGHT){

}
void LCD_I2C::begin(){
    sync_globals();
    Wire.begin();
    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delay(50); 
    // Now we pull both RS and R/W low to begin commands
    Wire.beginTransmission(_addr);
    Wire.write(_bl);
    Wire.endTransmission();
    delay(50);
    Wire.beginTransmission(_addr);
    Wire.write(_bl);
    Wire.endTransmission();
    delay(50);
    //put the LCD into 4 bit mode
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46
    // we start in 8bit mode, try to set 4 bit mode
    lcd_write4bits(0x03 << 4);
    delay(5); // wait min 4.1ms
    // second try
    lcd_write4bits(0x03 << 4);
    delay(5); // wait min 4.1ms
    // third go!
    lcd_write4bits(0x03 << 4); 
    delay(2);
    // finally, set to 4-bit interface
    lcd_write4bits(0x02 << 4); 
    // set # lines, font size, etc.
    
    _functions =  LCD_4BITMODE | LCD_1LINE | LCD_2LINE| LCD_5x8DOTS;
    lcd_send(LCD_FUNCTIONSET | _functions, 0);  
    // turn the display on with no cursor or blinking default
    _control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    // clear it off
    clear();
    // Initialize to default text direction (for roman languages)
    _mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    lcd_send(LCD_ENTRYMODESET | _mode, 0);
  
    home();
}
void LCD_I2C::sync_globals(){
    g_lcd_addr = _addr;
    g_lcd_bl = _bl;
}
void LCD_I2C::home(){
   sync_globals();
   lcd_send(LCD_RETURNHOME, 0);  // set cursor position to zero
   delayMicroseconds(2000);  // this command takes a long time!
}
void LCD_I2C::clear(){
  sync_globals();
  lcd_send(LCD_CLEARDISPLAY, 0);// clear display, set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}
void LCD_I2C::display(){
    sync_globals();
    _control |= LCD_DISPLAYON;
    lcd_send(LCD_DISPLAYCONTROL | _control, 0);
}
void LCD_I2C::noDisplay(){
   sync_globals();
   _control &= ~LCD_DISPLAYON;
   lcd_send(LCD_DISPLAYCONTROL | _control, 0);
}
void LCD_I2C::gotoXY(byte x,byte y){
    sync_globals();
    lcd_send(LCD_SETDDRAMADDR | (x + SYMB_SHIFT[y]), 0);
}
void LCD_I2C::write(uint8_t b){
    sync_globals();
    lcd_send(b, Rs);
}

byte g_lcd_bl;
byte g_lcd_addr;
