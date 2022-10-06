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
bool isReload = true;
float temp = 110;
int secondsOfMesurment = 1;
int counterCycle = 1;
bool endMesurment = false;




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
    isMesurment = false;
    isReload = false;
    delay(timeDelay);
   
  }


 int counter = 0;

void Task1code( void * pvParameters ){

 
  for(;;){
   delay(1000);
    //temp = dht.readTemperature();
    //

    if(counterCycle>2){
      endMesurment = true;
      isReload = false;
    }
        Serial.println("Cycle Number");
        Serial.println(counterCycle);
    if(isMesurment && !endMesurment ){
        
        temp = analogRead(39);
        secondsOfMesurment = counter;
        arr[counter] = temp;
        counter++;
      if(counter > 90){
      counter = 0;
      counterCycle++;
      //todoFunc for fecth Data
      //todo next steps
      getDelaySecondCycle(60 * 1000);
      memset(arr,0,90 * sizeof(int));
      isMesurment = true;
      isReload = true;
      }
    }
    

  
      

    Serial.print("val : ");
    Serial.println(temp);
    
  vTaskDelay(1000); 
  } 
}





void Task2code( void * pvParameters ){
 // Serial.print("Task1 running on core ");
 // Serial.println(xPortGetCoreID());

  for(;;){
       
 startServer( counter, counterCycle, isMesurment, isReload,endMesurment);
       
}
}


void loop() {


}
