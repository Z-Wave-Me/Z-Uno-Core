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
 */

//#include <string.h>
//#include <math.h>
//#include "Arduino.h"

#include "Print.h"

#define JUST_NEW_LINE write('\r');write('\n');
#define NEW_LINE_ADDN 2 


size_t  g_n; 
uint8_t g_i;
///

// Public Methods //////////////////////////////////////////////////////////////

/* default implementation: may be overridden */
size_t Print::write(uint8_t *buffer, size_t size)
{
  g_n = size;
  while (size--) {
     write(*buffer++);
  }
  return g_n;
}

size_t Print::print(char * str)
{

  for(g_i=0;g_i<0xff;g_i++)
  {
      if(str[g_i] == 0)
        break;
      write(str[g_i]);
  }
  return g_i;
  
}

size_t Print::print(char c)
{
  return write(c);
}

size_t Print::print(unsigned char b, int base)
{
  return printNumber((unsigned long)b, base);
}

size_t Print::print(int n, int base)
{
  byte add_sz = 0;
  if (base == 0) 
    return write(n);
  if ((base == 10) && (n < 0)) 
  {
      n = -n;
      write('-');
      add_sz=1;
  }

  return printNumber((unsigned long)n, base) + add_sz; 
}

size_t Print::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t Print::print(long n, int base)
{
  byte add_sz = 0;
  if (base == 0) 
    return write(n);
  if ((base == 10) && (n < 0)) 
  {
      n = -n;
      write('-');
      add_sz=1;
  }

  return printNumber(n, base) + add_sz; 
  
}

size_t Print::print(unsigned long n, int base)
{
  if (base == 0) 
    return write(n);
  return printNumber(n, base);
  
}

size_t Print::print(float n, int digits)
{
  return printFloat(n, digits);
}
size_t Print::print(double n, int digits)
{
  return printFloat((double)n, digits);
}
size_t Print::println(void)
{
  JUST_NEW_LINE;
  return NEW_LINE_ADDN;
}

size_t Print::println(char * c)
{
  

  print(c);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
  
}

size_t Print::println(char c)
{
  write(c);
  JUST_NEW_LINE;
  return 1+NEW_LINE_ADDN;
}

size_t Print::println(unsigned char b, int base)
{
  print(b, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

size_t Print::println(int num, int base)
{
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

size_t Print::println(unsigned int num, int base)
{
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

size_t Print::println(long num, int base)
{
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

size_t Print::println(unsigned long num, int base)
{
  print(num, base);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}

size_t Print::println(float num, int digits)
{
  printFloat(num, digits);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
size_t Print::println(double num, int digits)
{
  printFloat((float)num, digits);
  JUST_NEW_LINE;
  return g_n+NEW_LINE_ADDN;
}
// Private Methods /////////////////////////////////////////////////////////////

uint8_t g_digit_buffer[32];
uint8_t g_digit_len = 0;

unsigned long g_long_tmp = 0;
float         g_float_tmp = 0;



size_t Print::printNumber(unsigned long n, uint8_t base) {

  g_digit_len = 0;

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do 
  {
    g_i = n % base;
    n /= base;
    g_digit_buffer[g_digit_len++] = g_i < 10 ? g_i + '0' : g_i + 'A' - 10;
  } while(n);


  g_n = g_digit_len;

  // Печатаем в обратном порядке
  while(g_digit_len--)
      write(g_digit_buffer[g_digit_len]);         

  return g_n;
}

size_t Print::printFloat(float number, uint8_t digits) 
{ 
  g_n = 0;
  
  // Handle negative numbers
  if (number < 0.0)
  {
     g_n++;
     write('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  g_float_tmp = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    g_float_tmp /= 10.0;
  
  number += g_float_tmp;

  // Extract the integer part of the number and print it
  g_long_tmp = (unsigned long)number;
  g_float_tmp = number - (float)g_long_tmp;
  g_n += printNumber((unsigned long)g_long_tmp, 10);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    g_n++;
    write('.');
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    g_float_tmp *= 10.0;
    g_digit_buffer[0] = uint8_t(g_float_tmp);
    write('0'+ g_digit_buffer[0]);
    g_n++;
    g_float_tmp -= g_digit_buffer[0]; 
  } 
   
  return g_n;
}
