#pragma once

#include "Stream.h"

class HardwareSerial: public Stream
{
	private:
		BYTE   func_vec;

	public:

		HardwareSerial(BYTE bfn);  // bfn = like ZUNO_FUNC_SERIAL1_BEGIN

    	void begin();
    	void begin(DWORD baud);
    	void end();
    	virtual int available(void);
    	virtual int peek(void);
    	virtual int read(void);
   
   		virtual void flush(void);
    	virtual size_t write(uint8_t);

    	size_t write(unsigned long n) { return write((uint8_t)n); }
    	size_t write(long n) { return write((uint8_t)n); }
    	size_t write(unsigned int n) { return write((uint8_t)n); }
    	size_t write(int n) { return write((uint8_t)n); }


};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial0;
