#pragma once
#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"
// Здесь добавляются все библиотеки для которых необходимы плюсы...
#include "HardwareSerial.h" // Поддержка Serial
// Все обычные С-функции
#include "ArduinoCAPI.h"
#include "HLCore.h"

// Хидеры SDCC
#include "math.h"



// В SDCC все math-функции только float
// Преобразуем к привычным прототипам
#define sin(x) 			sinf(x)
#define cos(x) 			cosf(x)
#define tan(x) 			tanf(x)
#define cot(x) 			cotf(x)
#define asin(x) 		asinf(x)
#define acos(x) 		acosf(x)
#define atan(x) 		atanf(x)
#define atan2(x, y) 	atan2f(x, y)
#define sinh(x) 		sinhf(x)
#define cosh(x) 		coshf(x)
#define tanh(x) 		tanhf(x)
#define exp(x) 			expf(x)
#define log(x) 			logf(x)
#define log10(x) 		log10f(x)
#define pow(x,y) 		powf(x,y)
#define sqrt(x) 		sqrtf(x)
#define fabs(x)     	fabsf(x)
#define ceil(x)     	ceilf(x)
#define floor(x)    	floorf(x)
#define modf(x,y)   	modff(x,y)
#define frexp(x, pw2)   frexpf(x, pw2)
#define ldexp(x, pw2)   ldexpf(x, pw2)




//**********************************************
void setup(void);
void loop(void);


