float volts_midArifm_pwmSig(int num_countdowns = 100) { //напруга на транзисторі
  float sum = zero;
  int16_t adc;
  float volts;                      
  for (int i = 0; i < num_countdowns; i++)  
    sum += ads.readADC_SingleEnded(pin_voltageOntransistor);                  
  volts = ads.computeVolts(sum / num_countdowns);
  return volts;
}