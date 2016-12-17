#include <Wire.h>
#include <ZUNO_MCP4725.h>

#define voltInput A0

ZUNO_MCP4725 dac; // constructor

void setup(void) {
  Serial.begin(0);
  dac.begin(0x60); // The I2C Address: Run the I2C Scanner if you're not sure  
  
}

void loop(void) {
  
    uint32_t dac_val;
    int raw_adcRead = 0;
    float voltRead = 0;
    
    float calculated_output;
    
   
    for (dac_val = 0; dac_val < 4096; dac_val = dac_val + 15)
    {
      calculated_output = (5.0/4096.0) * dac_val;
      dac.setVoltage(dac_val, false);
      delay(250);
      raw_adcRead = analogRead(voltInput);
      voltRead = (raw_adcRead * 5.0 )/ 1024.0;
      
      Serial.print("MCP4725 DAC Value: ");
      Serial.print(dac_val);
      
      Serial.print("\tCalculated Voltage: ");
      Serial.print(calculated_output,3);
      
      Serial.print("\tADC Value (Z-Uno input): ");
      Serial.print(raw_adcRead);
      
      Serial.print("\tZ-Uno Voltage: ");      
      Serial.println(voltRead,3);      
    }    
}
