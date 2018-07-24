#include <Tone.h>
s_pin  g_tone_pin_ch1 = 11;
dword  g_tone_tmp;
ZUNO_SETUP_ISR_GPTIMER(tone_gpt_handler);

void toneDelayed(s_pin pin, word freq, word duration){
     tone(pin, freq);
     delay(duration);
     noTone(pin);
}

void tone(s_pin pin, word freq){
    zunoGPTEnable(0); 
    g_tone_tmp = 2000000L;
    g_tone_tmp /= freq;
    g_tone_pin_ch1 = pin;
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    zunoGPTInit(ZUNO_GPT_CYCLIC);   
    zunoGPTSet(word(g_tone_tmp)); 
    zunoGPTEnable(1); 
}
void noTone(s_pin pin){
    zunoGPTEnable(0); 
    digitalWrite(g_tone_pin_ch1, LOW);
}
void tone_gpt_handler()
{
   digitalToggle(g_tone_pin_ch1);

}