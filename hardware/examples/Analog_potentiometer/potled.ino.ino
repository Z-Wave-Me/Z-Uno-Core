#define LED_PIN 16
#define POT_PIN 6
BYTE last_value;
BYTE current_value;
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_MULTILEVEL(setter,getter))
ZUNO_SETUP_ASSOCIATIONS(ZUNO_ASSOCIATION_GROUP_SET_VALUE) 
ZUNO_SETUP_SLEEPING_MODE(ZUNO_SLEEPING_MODE_ALWAYS_AWAKE)
void setup()
{

  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

}
void loop()
  {
    BYTE current_value = (BYTE) (analogRead(2) / 4);
    analogWrite(4, current_value);
     if (current_value != last_value) 
  {
    last_value = current_value;
    zunoSendZWaveReport(1);
  }
  }
  



void setter(){
}
BYTE getter(void){
  BYTE perem;
  perem = (BYTE)((((DWORD) current_value)*100)/0xff);
  return perem;
}
