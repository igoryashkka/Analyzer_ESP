#include "web_server.h"
#include "main.h"
#include "logic_esp.h"



TaskHandle_t Task1;
 
TaskHandle_t Task2;
//____________________________________________________
#define period1 60*1000L
#define period2 90*1000L
#define period3 1000L
#define period4 3000L
uint32_t tmr3;
uint32_t tmr;
uint32_t tmr2;
bool flag = false;



//_____________________________________________________


//_____________________________________________________

int arr[90]={0};
int arrayDiff[90]={0};
int ppm1;
int ppm2;

int DutyCycle = 0;
int mq7_Ro1;
int mq7_Ro2;
//arduino_git
//arduino_ghub
//_____________________________________________________

//  test markup

bool isMesurment = true;
bool isReabsorb = false;
bool isReloadPage = true;
int counterCycle = 1;
int counterCycleFromUser = 3;
int counterSeconds = 0;
int timAbsorb = 90;
int timDeabsorb = 60;


const unsigned long periodMesurmentV = 20 * 1000L;// or absorb
const unsigned long periodDeabsorbV = 10 * 1000L;
const unsigned long oneSecondV = 1000L;
uint32_t timerDetectProcces; 
uint32_t timerEverySecond;  
bool flagDetectProcces = false;




//_____________________________________________________

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    initESP();

  initADC();
  dutyCycleOfPWM();
  setupSensors();





xTaskCreatePinnedToCore(Task1code,"server",10000,NULL,1,&Task1,0);                            
  delay(500); 
xTaskCreatePinnedToCore(Task2code,"main_logic",10000,NULL,1,&Task2,1);
  delay(500);
  
 
}


void getDelaySecondCycle(int timeDelay){
    isReabsorb = true;
    isMesurment = false;
    isReloadPage = false;
    delay(timeDelay);
   
  }
// transmit [1000l] in if ...
//defently long ? or mabe int or mabe uint ?


void Task1code( void * pvParameters ){

 
  for(;;){


      
     Serial.println("Befor if timerDetectProcces");
    if (millis() - timerDetectProcces >= (flagDetectProcces ? periodMesurmentV : periodDeabsorbV)) {
      timerDetectProcces = millis();
      // TODO SWITCH
      Serial.println("in if timerDetectProcces _ 0");
      if(flagDetectProcces==0)
      {
        ledcWrite(0, 255);
        Serial.println("log_0");
      }
      if(flagDetectProcces==1)
      {
        ledcWrite(0, DutyCycle);//DutyCycle????
        Serial.println("log_1");
      }
     flagDetectProcces = !flagDetectProcces;
     Serial.println("in if timerDetectProcces _ 1");
    }
     Serial.println("after if timerDetectProcces");

    if (millis() - timerEverySecond >= oneSecondV)
    { 
      timerEverySecond = millis();
      ppm1 = get_rawValue_mq7(mq7_Ro1, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO1);
      ppm2 = get_rawValue_mq7(mq7_Ro2, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO2);
      Serial.print("ppm1: ");  
   //   Serial.print(ppm1 );
      Serial.print("; ppm2: ");  
  //    Serial.print(ppm2 );
      Serial.print("; deltaPPM: ");  
      //Serial.println(ppm2-ppm1 );
      //Serial.println(temp);
      arr[counterSeconds] = ppm1;
      counterSeconds++;
      if(counterSeconds>90){
        counterSeconds = 0;
        counterCycle++;
        }
        Serial.print("arr[counterSeconds]");
      Serial.print(arr[counterSeconds]);
      
    }



    //if button did not press - not measurement
    //if button press - do [oneCycle * counterCycleFromUser] ; oneCycle is 90s(measurement or absorb) + 60s(reabsorb)
    /*
    if(counterCycle<=counterCycleFromUser){      
      if(isMesurment){
        arr[counterSeconds] = analogRead(39);
        delay(1000);
        counterSeconds++; 
          if(counterSeconds > 30){
            counterSeconds = 0;
            getDelaySecondCycle(10 * 1000);
            memset(arr,0,90 * sizeof(int));
            isReabsorb = false;
            isMesurment = true;
            isReloadPage = true;
            counterCycle++;
          }
      }
    }else
    { 
         isReabsorb =false; 
          isMesurment = false;
            isReloadPage = false;
      }
  */
   


  vTaskDelay(1000); 
  } 
}





void Task2code( void * pvParameters ){

  for(;;){
       
    startServer( counterSeconds, counterCycle, isMesurment, isReloadPage,isReabsorb);
       vTaskDelay(1000); 
  }
}


void loop() {


}
