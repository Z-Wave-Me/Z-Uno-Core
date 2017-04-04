/*
 *   This sketch tests stack depth
 */
// LED pin number
#define LED_PIN 13

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC (Serial)
#define MY_SERIAL Serial

void setup() 
{
  pinMode(LED_PIN, OUTPUT);      // set LED pin as output
  MY_SERIAL.begin(115200);
  MY_SERIAL.println("\n **** Sketch is starting... ****\n");
  digitalWrite(LED_PIN, LOW);

}

void testStack()
{
   static int g_counter = 0;
   MY_SERIAL.print("\n\t>>> ZUNO loop #");
   MY_SERIAL.print(g_counter, HEX);
   MY_SERIAL.print(" <<<\n"); 

   g_counter++;

   return; 
}
//  Рекурсивная функция, проверяющая возможности стека
void testStackR(byte max_deep)
{

    MY_SERIAL.print("\n Deep remains:");
    MY_SERIAL.println(max_deep, DEC);
    MY_SERIAL.print("\n Deep trigonometry:");
    MY_SERIAL.println(sin(0.01157894*max_deep),7);

    if(!max_deep)
    {
        MY_SERIAL.print("\n\n  @@@ Hello world from deep stack! @@@\n\n");
        return;  
    }

    testStackR(max_deep - 1);
}

void loop() 
{
  
  
  float sin_res = sin(PI*0.25);//0.7108456;//sinf(3.14*0.25);
  
  MY_SERIAL.print("Millis:");
  MY_SERIAL.println(millis());
  
  MY_SERIAL.print("Some float:");
  MY_SERIAL.println(sin_res);
  MY_SERIAL.println(sin_res,7);
  
  digitalWrite(LED_PIN, HIGH);   // turn LED on
  delay(1000);                   // wait for 1 second
  digitalWrite(LED_PIN, LOW);    // turn LED off
  testStack();
  delay(1000); 

  testStackR(10); 
  
}
