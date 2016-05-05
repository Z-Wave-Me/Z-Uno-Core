/*
  Print.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

  Simplified for Z-Uno project by Z-Wave.Me (c) 2016
*/

#ifndef Print_h
#define Print_h

#include "ArduinoTypes.h"
#include "c_lib_minimal.h"  


#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print
{
  private:
    int     write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(float, uint8_t);
    size_t printByteArr(uint8_t * bytearr, uint8_t len, uint8_t base);
  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() { write_error = 0; }
  
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
  
    virtual size_t write(uint8_t a){ return 0; };// = 0;
    virtual size_t write(uint8_t *buffer, size_t size);
    
    size_t print(char *);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(float, int = 2);

    size_t println(char *);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(float, int = 2);
    size_t println(void);
};

#endif
