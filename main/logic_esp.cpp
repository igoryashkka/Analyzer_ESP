#include "logic_esp.h"



void calibSensors(){
 ledcWrite(0, 255);
  delay(1000);
  ledcWrite(0, DutyCycle);
  delay(1000);
  mq7_Ro1 = calib_mq7_Ro(pin_voltageOn_CO1);
  mq7_Ro2 = calib_mq7_Ro(pin_voltageOn_CO2);
 
  
  Serial.print("mq7_Ro1: ");
  Serial.print(mq7_Ro1);
  Serial.print("; mq7_Ro2: ");
  Serial.println(mq7_Ro2);
}
//____________________________________________________

void initADC(){
Serial.println("Hello!");
  
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

}
//____________________________________________________
float volts_midArifm_pwmSig(int num_countdowns) { //напруга на транзисторі
  float sum = 0;
  int16_t adc;
  float volts;                      
  for (int i = 0; i < num_countdowns; i++)  
    sum += ads.readADC_SingleEnded(pin_voltageOntransistor);                  
  volts = ads.computeVolts(sum / num_countdowns);
  return volts;
}

void dutyCycleOfPWM() { // калібруємо напругу на транзисторія для режиму вимірювання
  const int freq = 100;
  const int ledChannel = 0;
  const int resolution = 8;
  bool flag_correct_dutyCycle = 0;
  
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(PWM_SIG, ledChannel);

  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledChannel, dutyCycle);
      //delay(50);
      
      float volt_level = volts_midArifm_pwmSig();
      Serial.println(volt_level); 
      if(volt_level >= 3.55 && volt_level <= 3.6 )
      {
        DutyCycle = dutyCycle; // ALERT!!!!! CHANGING GLOBAL VAR!!!!!!!!!!!!!!!!!!!
        Serial.print("voltage: ");  
        Serial.println(volt_level);
        Serial.println(DutyCycle);
        flag_correct_dutyCycle = 1;
        break;
      }
      
    }
    if(flag_correct_dutyCycle == 0)
    {
      Serial.println("not correct DutyCycle. Pls restart CO_analisator");
      while(1);
    }
}

int calib_mq7_Ro(int pin_voltageOn)
{
  float mq7_adc_volts;
  int mq7_adc;
  int Rs;
  int Ro;

  mq7_adc = ads.readADC_SingleEnded(pin_voltageOn);
  mq7_adc_volts = ads.computeVolts(mq7_adc);
  Rs = (((float)MQ7_REFERENCE_VOLTAGE * (float)MQ7_LOAD_RESISTOR)/mq7_adc_volts)- (float)MQ7_LOAD_RESISTOR;
  Ro = Rs/MQ7_CLEANAIR_RSRO;

  Serial.print("Rs: ");
  Serial.print(Rs);
  Serial.print("; volt_adc: ");
  Serial.println(mq7_adc_volts);
  
  return Ro;
}

int get_rawValue_mq7(int Ro, float reference_voltage_value, float pinCO)
{ 
  int ppm;
  int Rs;
  float RsRo;
  float mq7_adc_volts;
  int mq7_adc;
  mq7_adc = ads.readADC_SingleEnded(pinCO);
  mq7_adc_volts = ads.computeVolts(mq7_adc);
  Rs = (((float)MQ7_REFERENCE_VOLTAGE * (float)MQ7_LOAD_RESISTOR)/mq7_adc_volts)- (float)MQ7_LOAD_RESISTOR;
  RsRo = (float)Rs/(float)Ro; 
  if(RsRo>0.09) {
    ppm=(pow((0.196/RsRo),(1/0.72)))*1000;
  }
  else {ppm=3999;}

  return (float)RsRo;

}
