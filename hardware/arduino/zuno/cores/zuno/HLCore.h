#pragma once

extern BYTE g_gpio_pin;
extern BYTE g_gpio_val;
extern WORD g_gpio_wval;

DWORD pulseIn(s_pin pin, byte level, DWORD timeout);
long map(long x, long in_min, long in_max, long out_min, long out_max);

void delayMicroseconds(word us);
