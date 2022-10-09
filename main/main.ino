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
bool showSummaryPPM = false;
bool isMesurment = false;
bool isReabsorb = true;
bool isReloadPage = true;
int counterCycle = 1;
int counterCycleFromUser = 3;
int counterSeconds = 0;


// transmit [1000l] in if ...
//defently long ? or mabe int or mabe uint ?
//try to use not const
const unsigned long periodMesurmentV = 90 * 1000L;// or absorb
const unsigned long periodDeabsorbV = 60 * 1000L;
const unsigned long oneSecondV = 1000L;
uint32_t timerDetectProcces; 
uint32_t timerEverySecond;  
bool flagDetectProcces = false;


//_____________________________________________________

void setup() {
  Serial.begin(115200);
  initESP();

  //initADC();
  //dutyCycleOfPWM();
  //setupSensors();





  xTaskCreatePinnedToCore(Task1code,"server",10000,NULL,1,&Task1,0);                            
  delay(500); 
  xTaskCreatePinnedToCore(Task2code,"main_logic",10000,NULL,1,&Task2,1);
  delay(500);
  
 
}





void Task1code( void * pvParameters ){
  for(;;){


        //if button did not press - not measurement
    //if button press - do [oneCycle * counterCycleFromUser] ; oneCycle is 90s(measurement or absorb) + 60s(reabsorb)
   
  if(counterCycle<counterCycleFromUser){

    if (millis() - timerDetectProcces >= (flagDetectProcces ? periodMesurmentV : periodDeabsorbV)) {
      timerDetectProcces = millis();
      // TODO SWITCH
    
      
        
      if(flagDetectProcces==0)
      {
        isReabsorb = false;
       // ledcWrite(0, 255);
        Serial.println("log_0");
      }else{isReabsorb = true; }
      if(flagDetectProcces==1)
      {
        isMesurment = false;
        //ledcWrite(0, DutyCycle);//DutyCycle????
        Serial.println("log_1");
      }else {isMesurment = true;}

      flagDetectProcces = !flagDetectProcces;
     
    }
     

    if ((millis() - timerEverySecond >= oneSecondV) && isMesurment)
    { 
      timerEverySecond = millis();
    //  ppm1 = get_rawValue_mq7(mq7_Ro1, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO1);
    //  ppm2 = get_rawValue_mq7(mq7_Ro2, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO2);

      arr[counterSeconds] = analogRead(39);

   //   Serial.print("ppm1: ");  
   //   Serial.print(ppm1 );
   //   Serial.print("; ppm2: ");  
  //    Serial.print(ppm2 );
   //   Serial.print("; deltaPPM: ");  
      //Serial.println(ppm2-ppm1 );
      //Serial.println(temp);
   //   arr[counterSeconds] = ppm1;
      counterSeconds++;
      if(counterSeconds>90){
        counterSeconds = 0;
        counterCycle++;
        memset(arr,0,90 * sizeof(int));
        }
        Serial.print("arr[counterSeconds]");
      Serial.print(arr[counterSeconds]);
      
    }
    showSummaryPPM = false;
  }else{
      showSummaryPPM = true;

  }


  

   


  vTaskDelay(1000); 
  } 
}





void Task2code( void * pvParameters ){

  for(;;){
       
    startServer( counterSeconds, counterCycle, isMesurment, isReloadPage,isReabsorb,showSummaryPPM);
       vTaskDelay(1000); 
  }
}


void loop() {


}
