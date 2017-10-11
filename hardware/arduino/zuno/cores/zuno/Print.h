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
    uint8_t dump_line_size;
  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() { write_error = 0; dump_line_size = 10;};
  
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
    void setDumpLine(uint8_t line_size) {dump_line_size = line_size;};


    virtual void write(uint8_t a){ };// = 0;
    virtual uint8_t write(uint8_t *buffer, size_t size);
    
    uint8_t print(char *);
    uint8_t print(char);
    uint8_t print(unsigned char, uint8_t = DEC);
    uint8_t print(int, uint8_t = DEC);
    uint8_t print(unsigned int, uint8_t = DEC);
    uint8_t print(long, uint8_t = DEC);
    uint8_t print(unsigned long, uint8_t = DEC);
    uint8_t print(float, uint8_t = 2);
    uint8_t print(double, uint8_t = 2);
    uint8_t fixPrint(long, uint8_t  = 2);
    uint8_t fixPrint(int n, uint8_t precision);
    word    dumpPrint(uint8_t *, uint8_t);
   


    uint8_t println(char *);
    uint8_t println(char);
    uint8_t println(unsigned char, uint8_t = DEC);
    uint8_t println(int, uint8_t = DEC);
    uint8_t println(unsigned int, uint8_t = DEC);
    uint8_t println(long, uint8_t = DEC);
    uint8_t println(unsigned long, uint8_t = DEC);
    uint8_t println(float, uint8_t = 2);
    uint8_t println(double, uint8_t = 2);
    
    
    uint8_t println(void);

};

#endif
