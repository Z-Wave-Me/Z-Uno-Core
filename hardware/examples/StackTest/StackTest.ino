/*
 * Скетч для тестирования корректности работы стека 
 * Также здесь проверяется работа с числами с плавающей запятой
 * Работа с потоками ввода/вывода Print/Stream/HardwareSerial
 *   #### Необходимо запускать при всех изменениях компилятора ####
 */
// LED pin number
#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);      // set LED pin as output
  Serial1.begin(115200);
  
  
  Serial1.println("\n **** Sketch is starting... ****\n");
  digitalWrite(LED_PIN, LOW);

}



void testStack()
{
   static int g_counter = 0;
   Serial1.print("\n\t>>> ZUNO loop #");
   Serial1.print(g_counter, HEX);
   Serial1.print(" <<<\n"); 

   g_counter++;

   return; 
}
//  Рекурсивная функция, проверяющая возможности стека
void testStackR(byte max_deep)
{

    Serial1.print("\n Deep remains:");
    Serial1.println(max_deep, DEC);
    Serial1.print("\n Deep trigonometry:");
    Serial1.println(sin(0.01157894*max_deep),7);

    if(!max_deep)
    {
        //printUART1Str("\n\n  @@@ Hello world from deep stack! @@@\n\n");
        Serial1.print("\n\n  @@@ Hello world from deep stack! @@@\n\n");
        return;  
    }
   

       
    testStackR(max_deep - 1);
}


void loop() {
  
  
  float sin_res = sin(PI*0.25);//0.7108456;//sinf(3.14*0.25);


  
  Serial1.print("Millis:");
  Serial1.println(millis());
  
  
  Serial1.print("Some float:");
  Serial1.println(sin_res);
  Serial1.println(sin_res,7);
  
  
  digitalWrite(LED_PIN, HIGH);   // turn LED on
  delay(1000);                   // wait for 1 second
  digitalWrite(LED_PIN, LOW);    // turn LED off
  testStack();
  delay(1000); 

  testStackR(10); 
  
}
