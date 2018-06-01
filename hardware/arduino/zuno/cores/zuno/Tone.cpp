#include <Tone.h>
s_pin  g_tone_pin_ch1 = 11;

ZUNO_SETUP_ISR_GPTIMER(tone_gpt_handler);

void toneDelayed(s_pin pin, word freq, word duration){
     tone(pin, freq);
     delay(duration);
     noTone(pin);
}

void tone(s_pin pin, word freq){
    dword ticks = 2000000L/freq;
    g_tone_pin_ch1 = pin;
    zunoGPTEnable(0); 
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    zunoGPTInit(ZUNO_GPT_CYCLIC);   
    zunoGPTSet(word(ticks)); 
    zunoGPTEnable(1); 
}
void noTone(s_pin pin){
    zunoGPTEnable(0); 
    digitalWrite(g_tone_pin_ch1, LOW);
}
void tone_gpt_handler()
{
   byte ts = !digitalRead(g_tone_pin_ch1);
   digitalWrite(g_tone_pin_ch1, ts);

}