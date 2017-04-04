
// This sketch demonstrates using of KeyPad 4x4 with Z-Uno
#include <ZMEKeypad.h> 
// Define number of rows
#define ROWS  4    
// Define number of columns        
#define COLS  4                 
                    


// Define pins numbers that we use for rows
BYTE rowPins[4] = {9, 10, 11, 12};
// Define pins numbers that we use for volumns
BYTE columnPins[4] = {14, 15, 16, 17};

// Titlse for all buttons in one string
char keys[] = "123A456B789C*0#D";



#define ledpin 13 
// Construct keypad object           
ZMEKeypad kpd = ZMEKeypad(columnPins, COLS, rowPins, ROWS);


void setup()
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, LOW);
  Serial.begin();
 
  // Init keypad
  kpd.begin();
  Serial.println("Sketch is starting...");
  
}

void loop()
{
  byte actions[4]; 
  byte num_touched_keys = kpd.scanKeys(actions);
 
  // We can process a number of buttons during one scan
  for(byte i=0;i<num_touched_keys;i++)
  {
      byte key_index = KEYPAD_KEYINDEX(actions[i]);

      // the button was held
      if(KEYPAD_IS_HOLDING(actions[i]))
      {
          Serial.print("HOLD ");
      }
      // the button was pressed
      else
      {
          Serial.print("PRESSED ");
          // Example of handling some buttons
          if(keys[key_index] == '*')
            digitalWrite(ledpin, HIGH);
          if(keys[key_index] == '#')
            digitalWrite(ledpin, LOW);
             
      }
      // Print info about pressed key to Serial
      Serial.print("KEY #");
      Serial.print(key_index);
      Serial.print(" caption: ");
      Serial.println(keys[key_index]);
   
  }

  delay(10);
}

