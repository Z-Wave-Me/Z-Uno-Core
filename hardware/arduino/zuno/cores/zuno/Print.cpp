/*
 Print.cpp - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Modified 23 November 2006 by David A. Mellis

 Simplified for Z-Uno project by Z-Wave.Me (c) 2016
 Reworked to reduce stack size by Z-Wave.Me (c) 2017
 */

#include "Print.h"
// ------------------------------------------------------------------------
// HELPER MACROSES
// ------------------------------------------------------------------------
#define JUST_NEW_LINE write('\r');write('\n');
#define NEW_LINE_ADDN 2 
#define NO_PRECISION 0xFF
#define WRITE_CHARS()\
                      while(g_digit_len--) \
                        write(g_digit_buffer[g_digit_len])
#define CHECH_SIGN(N,B)\
                      if ((B == 10) && (N < 0)) {\
                        N = -N;\
                        write('-');\
                        g_n++;\
                      }
#define BYTE2STR(N,B)\
                      g_byte_tmp = N;\
                      g_base = B;\
                      printBNumber();\
                      g_n += g_digit_len
#define WORD2STR(N,B)\
                      g_word_tmp = N;\
                      g_base = B;\
                      g_precision = NO_PRECISION;\
                      printWNumber();\
                      g_n += g_digit_len

#define LONG2STR(N,B)\
                     g_long_tmp = N; \
                     g_base = B; \
                     g_precision = NO_PRECISION;\
                     printLNumber();\
                     g_n += g_digit_len   
#define FIXEDPOINTLONG2STR(N,P)\
                     g_long_tmp = N; \
                     g_base = 10; \
                     g_precision = P;\
                     printLNumber();\
                     addLeadingZeros();\
                     g_n += g_digit_len
#define FIXEDPOINTWORD2STR(N,P)\
                     g_word_tmp = N; \
                     g_base = 10; \
                     g_precision = P;\
                     printWNumber();\
                     addLeadingZeros();\
                     g_n += g_digit_len
#define ADD_SYMBOL(C) g_digit_buffer[g_digit_len++] = C
#define CONV_DIGIT(V) V = (V > 9) ? V -10 + 'A' : V + '0'    
// ------------------------------------------------------------------------


extern uint8_t       g_digit_buffer[32];
extern uint8_t       g_digit_len;
extern uint8_t       g_base;
extern uint8_t       g_precision;
extern uint8_t       g_byte_tmp;
extern unsigned long g_long_tmp;
extern unsigned long g_long_tmp2;
extern word          g_int16_count;
extern word          g_word_tmp;
extern uint8_t       g_n; 
extern uint8_t       g_i;

// Auxilary methods that help to reduce stack usage
void printLNumber();
void printBNumber();
void printWNumber();
void addLeadingZeros();

// Public Methods //////////////////////////////////////////////////////////////

/* default implementation: may be overridden */
uint8_t Print::write(uint8_t *buffer, size_t size) {
  g_n = size;
  while (size--) {
     write(*buffer++);
  }
  return g_n;
}
uint8_t Print::print(char * str) {
  for(g_i=0;g_i<0xff;g_i++)
  {
      if(str[g_i] == 0)
        break;
      write(str[g_i]);
  }
  return g_i;
}
uint8_t Print::print(char c) {
  write(c);
  return 1;
}
uint8_t Print::print(unsigned char b, uint8_t base) {
  g_n = 0;
  BYTE2STR(b,base);
  WRITE_CHARS();
  return g_n;
}
uint8_t Print::print(int n, uint8_t base) {
  g_n = 0;
  CHECH_SIGN(n, base);
  WORD2STR(n, base);
  WRITE_CHARS();
  return g_n;
}
uint8_t Print::print(unsigned int n, uint8_t base) {
  g_n = 0;
  WORD2STR(n, base);
  WRITE_CHARS();
  return g_n;
}
uint8_t Print::print(long n, uint8_t base) {
  g_n = 0;
  CHECH_SIGN(n, base);
  LONG2STR(n, base);
  WRITE_CHARS();
  return g_n; 
}
uint8_t Print::print(unsigned long n, uint8_t base) {
  g_n = 0;
  LONG2STR(n, base);
  WRITE_CHARS();
  return g_n; 
}
uint8_t Print::print(float n, uint8_t digits) {
   g_n = 0;
   byte ch = digits;  
   g_long_tmp = 1;
   while(ch--)
      g_long_tmp *= 10;
   CHECH_SIGN(n, 10); 
   n *= g_long_tmp;
   FIXEDPOINTLONG2STR((long)(n), digits);
   WRITE_CHARS();
   return g_n;
}
uint8_t Print::print(double n, uint8_t digits) {
  return print((float)n, digits);
}
uint8_t Print::fixPrint(long n, uint8_t precision) {
    g_n = 0;
    CHECH_SIGN(n, 10);
    FIXEDPOINTLONG2STR(n, precision);
    WRITE_CHARS();
    return g_n;           
}
uint8_t Print::fixPrint(int n, uint8_t precision) {
    g_n = 0;
    CHECH_SIGN(n, 10);
    FIXEDPOINTWORD2STR(n, precision);
    WRITE_CHARS();
    return g_n;           
}
word Print::dumpPrint(uint8_t * b, uint8_t count) {
    byte ch;
    g_n = 0; 
    while(count--) {
        ch = *b;
        ch >>= 4;
        CONV_DIGIT(ch);
        write(ch);
        ch = *b;
        ch &= 0x0F;
        CONV_DIGIT(ch);
        write(ch);
        g_n++;
        if((g_n % dump_line_size) == 0)
          write('\n');
        else
          write(' ');
        b++;
    }
    g_int16_count = g_n;
    g_int16_count *= 3;
    return g_int16_count;
} 

uint8_t Print::println(void) {
  JUST_NEW_LINE;
  return NEW_LINE_ADDN;
}
uint8_t Print::println(char * c) {
  print(c);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(char c) {
  write(c);
  JUST_NEW_LINE;
  return 1+NEW_LINE_ADDN;
}
uint8_t Print::println(unsigned char b, uint8_t base) {
  print(b, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(int num, uint8_t base){
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(unsigned int num, uint8_t base) {
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(long num, uint8_t base) {
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(unsigned long num, uint8_t base) {
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(float num, uint8_t digits) {
  print(num, digits);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
uint8_t Print::println(double num, uint8_t digits) {
  print((float)num, digits);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

// Private Area /////////////////////////////////////////////////////////////
// Global varriabless
uint8_t g_digit_buffer[32];
uint8_t g_digit_len = 0;
uint8_t g_base;
uint8_t g_precision;
uint8_t g_byte_tmp;
char    g_char_tmp;
unsigned long g_long_tmp = 0;
word    g_int16_count;
word    g_word_tmp;
uint8_t  g_n; 
uint8_t g_i;

#define FIXBASE(B) if(B < 2) B = 10;
// We try to reduce a stack usage...
// Auxilary functions
// -------------------------------------------
// printLNumber()
// Conerts unsigned long to character representation
// stores it to g_digit_buffer
// -------------------------------------------
void printLNumber() {
    byte ch; 
    g_digit_len = 0;
    FIXBASE(g_base);
    do{
        // We use this to print fixed point numbers too
        // Check if we need to add the point
        if(g_digit_len == g_precision)
          ADD_SYMBOL('.');
        // Extract the last digit ftom the value
        ch = g_long_tmp % g_base;
        // Convert to character
        CONV_DIGIT(ch);
        // Go to the next digit
        g_long_tmp /= g_base;
        // Store character to buffer
        ADD_SYMBOL(ch);
     }while(g_long_tmp);
}
// -------------------------------------------
// printWNumber()
// Conerts unsigned int to character representation
// stores it to g_digit_buffer
// We can use only printLNumber function, but 
// this speed ups int operations an reduces 
// memory usage if you print only 16-bit values
// -------------------------------------------
void printWNumber() {
    byte ch; 
    g_digit_len = 0;
    FIXBASE(g_base);
    do{
        // We use this to print fixed point numbers too
        // Check if we need to add the point
        if(g_digit_len == g_precision)
          ADD_SYMBOL('.');
        // Extract the last digit ftom the value
        ch = g_word_tmp % g_base;
        // Convert to character
        CONV_DIGIT(ch);
        // Go to the next digit
        g_word_tmp /= g_base;
        // Store character to buffer
        ADD_SYMBOL(ch);
     }while(g_word_tmp);
}
// -------------------------------------------
// printBNumber()
// Conerts byte to characters representation
// stores them to g_digit_buffer
// We can use only printLNumber function, but 
// this speed ups byte operations an reduces 
// memory usage if you print only 8bit values
// -------------------------------------------
void printBNumber(){
    byte ch; 
    g_digit_len = 0;
    // fixing base
    FIXBASE(g_base);
    do{
        // Extract the last digit ftom the value
        ch = g_byte_tmp % g_base;
        // Convert to character
        CONV_DIGIT(ch);
        // Go to the next digit
        g_byte_tmp /= g_base;
        // Store character to buffer
        ADD_SYMBOL(ch);
     }while(g_byte_tmp);
}
// -------------------------------------------
// addLeadingZeros()
// Helper method for fix-point values formatting
// -------------------------------------------
void addLeadingZeros() {
  if((g_precision == NO_PRECISION) || (g_precision < g_digit_len))
    return;   
  g_precision -= g_digit_len;
  while(g_precision--)
      ADD_SYMBOL('0');
  ADD_SYMBOL('.');
  ADD_SYMBOL('0');         
  
}


