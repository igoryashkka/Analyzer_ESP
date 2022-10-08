#include "web_server.h"
#include "main.h"



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

int arr[90]={0};
int arrayDiff[90]={0};

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




//_____________________________________________________

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
    initESP();





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


 

void Task1code( void * pvParameters ){

 
  for(;;){

    //if button did not press - not measurement
    //if button press - do [oneCycle * counterCycleFromUser] ; oneCycle is 90s(measurement or absorb) + 60s(reabsorb)
    
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
  
    
  vTaskDelay(1000); 
  } 
}





void Task2code( void * pvParameters ){

  for(;;){
       
    startServer( counterSeconds, counterCycle, isMesurment, isReloadPage,isReabsorb);
       
  }
}


void loop() {


}
