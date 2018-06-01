#ifndef ARDUINO_LIKE_TYPES__
#define ARDUINO_LIKE_TYPES__
#include "ZUNO_Definitions.h"

typedef unsigned char 	boolean;
typedef unsigned char 	uint8_t;
typedef unsigned char 	s_pin;
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;

typedef char 	int8_t;
typedef int    int16_t;
typedef long   int32_t;


typedef unsigned int 	size_t;
typedef unsigned char   BYTE;
typedef unsigned int  	WORD;
typedef unsigned long   DWORD;
typedef unsigned char   XBYTE;
typedef unsigned char   CCBYTE;

typedef unsigned char   byte;
typedef unsigned int    word;
typedef unsigned long   dword;

#define bool 	BYTE

#define TRUE    1
#define FALSE   0

typedef unsigned long   time_t;

struct tmElements_s {
uint8_t Second;
uint8_t Minute;
uint8_t Hour;
uint8_t Wday; // day of week, sunday is day 1
uint8_t Day;
uint8_t Month;
uint8_t Year; // offset from 1970;
};


typedef struct tmElements_s tmElements_t;
typedef enum tmElementsFields{
    tmSecond, tmMinute, tmHour, tmWday, tmDay,tmMonth, tmYear, tmNbrFields
}tmElementsFields_t;	 


//#define true 	TRUE
//#define false	FALSE

#include "string.h"


#endif // ARDUINO_LIKE_TYPES__