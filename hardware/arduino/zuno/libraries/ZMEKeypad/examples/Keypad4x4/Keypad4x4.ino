
// Работа с KeyPad 4x4
#include <ZMEKeypad.h> 
// Количество строк
#define ROWS  4    
// Количество столбцов         
#define COLS  4                 
                    


// Определяем пины, которые подключены к строкам
BYTE rowPins[4] = {9, 10, 11, 12};
// Определяем пины, которые подключены к столбцам
BYTE columnPins[4] = {14, 15, 16, 17};

// Раскладываем весь keypad по строкам
// чтобы сделать подписи кнопок
char keys[] = "123A456B789C*0#D";



#define ledpin 13 
// Создаем объект KeyPad'a           
ZMEKeypad kpd = ZMEKeypad(columnPins, COLS, rowPins, ROWS);


void setup()
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, LOW);
  Serial.begin();
 
  // Инициализируем KeyPad
  kpd.begin();
  Serial.println("Sketch is starting...");
  
}

void loop()
{
  byte actions[4]; 
  byte num_touched_keys = kpd.scanKeys(actions);
 
  // Одновременно могут быть нажаты несколько клавиш
  // Перебираем весь список
  for(byte i=0;i<num_touched_keys;i++)
  {
      byte key_index = KEYPAD_KEYINDEX(actions[i]);

      // Удерживание
      if(KEYPAD_IS_HOLDING(actions[i]))
      {
          Serial.print("HOLD ");
      }
      // Нажатие
      else
      {
          Serial.print("PRESSED ");
          // Пример выполения действия по нажатию
          // клавиши
          if(keys[key_index] == '*')
            digitalWrite(ledpin, HIGH);
          if(keys[key_index] == '#')
            digitalWrite(ledpin, LOW);
             
      }
      // Распечатываем информацию о нажатой 
      // клавише в последовательный порт 
      Serial.print("KEY #");
      Serial.print(key_index);
      Serial.print(" caption: ");
      Serial.println(keys[key_index]);
   
  }

  delay(10);
}

