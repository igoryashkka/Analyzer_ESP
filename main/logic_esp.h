#include <Adafruit_ADS1X15.h>
//____________________________________________________
int aboba_func();
#define pin_voltageOntransistor 1 //напруга на транзисторі
#define pin_voltageOn_CO1 2 //напруга на першому датчику
#define pin_voltageOn_CO2 3 //напруга на другому датчику
#define MQ7_LOAD_RESISTOR 10000
#define MQ7_CLEANAIR_RSRO 11.7
#define MQ7_REFERENCE_VOLTAGE 3.3
#define MQ7_LOAD_RESISTOR 10000
#define PWM_SIG 15

//#define period3 1000L

//____________________________________________________
static Adafruit_ADS1015 ads;

static int name  = 32;
static int DutyCycle = 0;
static int mq7_Ro1, mq7_Ro2;
static int ppm1[90];
static int ppm2[90];
static int counter = 0;



//____________________________________________________  


float volts_midArifm_pwmSig(int num_countdowns = 100);
int calib_mq7_Ro(int pin_voltageOn);
int get_rawValue_mq7(int Ro, float reference_voltage_value, float pinCO);
void initADC();
void calibSensors();
void dutyCycleOfPWM();


//____________________________________________________  
int aboba_func();
