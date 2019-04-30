#pragma once

extern BYTE g_gpio_pin;
extern BYTE g_gpio_val;
extern WORD g_gpio_wval;
extern XBYTE g_nzram_NID;
extern XBYTE g_nzram_BeamCount;
extern XBYTE g_nzram_WUOpts;

extern unsigned long    g_ms_counter;

extern BYTE g_current_channel;
extern XBYTE * g_txbuff;
extern XBYTE * g_cmd;


DWORD pulseIn(s_pin pin, byte level, DWORD timeout);
long map(long x, long in_min, long in_max, long out_min, long out_max);

void delayMicroseconds(word us);
