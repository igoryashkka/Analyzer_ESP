#include <WiFi.h>
#include <Adafruit_ADS1X15.h>

//git init
//git status
//git add
//git commit -m "..."
//git branch
//git branch test
//git checkout test
//git merge 


//for commit
//1) git add to checkinig files
//2) if you modife data -> use git add again
//3)do commit
//---
//for create branches
//1)git branch test (create)
//2)git checkout test (change branch)
//
//Merge branches
//1)git merge test (add changes from test in master)
//
//git config --global user.name git config --global email.name
//git remote add origin https://github.com/igoryashkka/Analyzer_ESP.git
// git push -u origin master (push to master)
//git clone https://github.com/igoryashkka/Analyzer_ESP.git (for cloning)


//____________________________________________________
#define pin_voltageOntransistor 1 //напруга на транзисторі
#define pin_voltageOn_CO1 2 //напруга на першому датчику
#define pin_voltageOn_CO2 3 //напруга на другому датчику
#define period1 60*1000L
#define period2 90*1000L
#define MQ7_LOAD_RESISTOR 10000
#define MQ7_CLEANAIR_RSRO 11.7
#define MQ7_REFERENCE_VOLTAGE 3.3
#define MQ7_LOAD_RESISTOR 10000
#define PWM_SIG 15
#define period1 60*1000L
#define period2 90*1000L
#define period3 1000L
#define period4 3000L

Adafruit_ADS1015 ads;

//separate 
int DutyCycle = 0;
int mq7_Ro1, mq7_Ro2;
uint32_t tmr;
uint32_t tmr2;
uint32_t tmr3;
bool flag = false;
int ppm1[90];
int ppm2[90];
int counter = 0;
//_____________________________________________________

WiFiServer server(80);

int numberMeasurement = 1; 
String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;
int arr[90]={0};
int arrayDiff[90]={0};
const char* ssid = "lab-503";
const char* password = "OtYtDvthkfErhf]ybYsCkfdfYsDjkz";
//arduino_git
//arduino_ghub
//_____________________________________________________
float volts_midArifm_pwmSig(int num_countdowns = 100) { //напруга на транзисторі
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

//_____________________________________________________

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();





Serial.println("Hello!");
  
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }


  
  dutyCycleOfPWM();

  ledcWrite(0, 255);
  delay(period1);
   
  ledcWrite(0, DutyCycle);
  delay(1000);
  mq7_Ro1 = calib_mq7_Ro(pin_voltageOn_CO1);
  mq7_Ro2 = calib_mq7_Ro(pin_voltageOn_CO2);
 

  
  
  Serial.print("mq7_Ro1: ");
  Serial.print(mq7_Ro1);
  Serial.print("; mq7_Ro2: ");
  Serial.println(mq7_Ro2);
}

void loop() {

  if (millis() - tmr >= (flag ? period2 : period1)) {
    tmr = millis();
    
    // код
      if(flag==0)
      {
        ledcWrite(0, 255);
      }
      if(flag==1)
      {
        ledcWrite(0, DutyCycle);
      }
     flag = !flag;
    }
    if (millis() - tmr2 >= period3)
    { 
      
      tmr2 = millis();
      ppm1[counter] = get_rawValue_mq7(mq7_Ro1, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO1);
      ppm2[counter] = get_rawValue_mq7(mq7_Ro2, MQ7_REFERENCE_VOLTAGE, pin_voltageOn_CO2);
      arr[counter]=ppm1[counter];
      counter++;
      if(counter>90){
        counter = 0;
        }
      Serial.print("ppm1: ");  
      Serial.print(ppm1[counter] );
      //Serial.print("; ppm2: ");  
      //Serial.print(ppm2 );
     // Serial.print("; deltaPPM: ");  
      //Serial.println(ppm2-ppm1 );
    }

  if(millis() - tmr3 >= period4){
     tmr3 = millis();
 
  WiFiClient client = server.available();   

  if (client) {                            
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                  
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>");
            client.println("<style>table { font-family: arial, sans-serif; border-collapse: collapse; width: 100%; } td, th { border: 1px solid #202142; text-align: left; padding: 8px; } tr:nth-child(even) { background-color: rgba(0,0,0,0.4); } * { box-sizing: border-box; } body { font-family: 'Poppins', sans-serif; background-color: #202142; color: white; /* background: #f1f1f1; */ } /* Header/Blog Title rgba(0, 0, 0, 0.4) */ .header { padding: 30px; text-align: center; background-color: rgba(0,0,0,0.4); } .header h1 { font-size: 50px; color:rgb(69, 114, 167); } .header p{ color:rgb(69, 114, 167); } .button{ font-family: 'Poppins', sans-serif; font-size: 25px; color: white; width: 100%; height: 100%; background-color:#202142; border: none; } .button:hover{ color :rgb(69, 100, 100); } .leftcolumn { float: left; width: 75%; background-color: rgba(0, 0, 0, 0.4); } .rightcolumn { float: left; width: 25%; background-color: rgba(0, 0, 0, 0.4); } .fakeimg0 { background-color: #202142; width: 100%; } .fakeimg { background-color: #202142; width: 100%; padding: 20px; } /* Add a card effect for articles */ .card { background-color: rgba(0, 0, 0, 0.4); padding: 20px; margin-top: 20px; } .row:after { content: \"\"; display: table; clear: both; } /* Footer */ .footer { padding: 20px; text-align: center; background-color: rgba(0,0,0,0.4); margin-top: 5px; color:rgb(69, 114, 167); } /* Responsive layout - when the screen is less than 800px wide, make the two columns stack on top of each other instead of next to each other */ @media screen and (max-width: 800px) { .leftcolumn, .rightcolumn { width: 100%; padding: 0; } } /* Responsive layout - when the screen is less than 400px wide, make the navigation links stack on top of each other instead of next to each other */ @media screen and (max-width: 400px) { .topnav a { float: none; width: 100%; } } ");
            client.println("</style></head>");
            client.println("");

            client.println("<body><div class=\"header\"> <h1>CO Analyzer</h1> <p>Built on ESP32</p> </div>");
            client.println("<div class=\"row\"> <div class=\"leftcolumn\"> <div class=\"card\"> <h2>Data processing from CO analyzer is presented in the form of a graph.</h2> <h5> The measurement cycle is 90 seconds. Number of cycle");
            //client.println(numberMeasurement);
            client.println("</h5> <div class=\"fakeimg\" style=\"height:800px;\">");
            client.println("<div> <canvas id=\"myChart\"></canvas> </div> <script> const labels = [ '00:01','00:02','00:03','00:04','00:05','00:06','00:07','00:08','00:09','00:10','00:11','00:12','00:13','00:14','00:15','00:16','00:17','00:18','00:19','00:20','00:21','00:22','00:23','00:24','00:25','00:26','00:27','00:28','00:29','00:30','00:31','00:32','00:33','00:34','00:35','00:36','00:37','00:38','00:39','00:40','00:41','00:42','00:43','00:44','00:45','00:46','00:47','00:48','00:49','00:50','00:51','00:52','00:53','00:54','00:55','00:56','00:57','00:58','00:59','01:00','01:01','01:02','01:03','01:04','01:05','01:06','01:07','01:08','01:09','01:10','01:11','01:12','01:13','01:14','01:15','01:16','01:17','01:18','01:19','01:20','01:21','01:22','01:23','01:24','01:25','01:26','01:27','01:28','01:29','01:30', ]; const data = { labels: labels, datasets: [{ label: 'My First dataset', backgroundColor: 'rgb(255, 99, 132)', borderColor: 'rgb(255, 99, 132)',");
            client.println("  data: [");
            client.println(arr[0]); client.println(",");client.println(arr[1]); client.println(",");client.println(arr[2]); client.println(",");client.println(arr[3]); client.println(",");client.println(arr[4]); client.println(",");client.println(arr[5]); client.println(",");client.println(arr[6]); client.println(",");client.println(arr[7]); client.println(",");client.println(arr[8]); client.println(",");client.println(arr[9]); client.println(",");
            client.println(arr[10]); client.println(",");client.println(arr[11]); client.println(",");client.println(arr[12]); client.println(",");client.println(arr[13]); client.println(",");client.println(arr[14]); client.println(",");client.println(arr[15]); client.println(",");client.println(arr[16]); client.println(",");client.println(arr[17]); client.println(",");client.println(arr[18]); client.println(",");client.println(arr[19]); client.println(",");
            client.println(arr[20]); client.println(",");client.println(arr[21]); client.println(",");client.println(arr[22]); client.println(",");client.println(arr[23]); client.println(",");client.println(arr[24]); client.println(",");client.println(arr[25]); client.println(",");client.println(arr[26]); client.println(",");client.println(arr[27]); client.println(",");client.println(arr[28]); client.println(",");client.println(arr[29]); client.println(",");
            client.println(arr[30]); client.println(",");client.println(arr[31]); client.println(",");client.println(arr[32]); client.println(",");client.println(arr[33]); client.println(",");client.println(arr[34]); client.println(",");client.println(arr[35]); client.println(",");client.println(arr[36]); client.println(",");client.println(arr[37]); client.println(",");client.println(arr[38]); client.println(",");client.println(arr[39]); client.println(",");
            client.println(arr[40]); client.println(",");client.println(arr[41]); client.println(",");client.println(arr[42]); client.println(",");client.println(arr[43]); client.println(",");client.println(arr[44]); client.println(",");client.println(arr[45]); client.println(",");client.println(arr[46]); client.println(",");client.println(arr[47]); client.println(",");client.println(arr[48]); client.println(",");client.println(arr[49]); client.println(",");
            client.println(arr[50]); client.println(",");client.println(arr[51]); client.println(",");client.println(arr[52]); client.println(",");client.println(arr[53]); client.println(",");client.println(arr[54]); client.println(",");client.println(arr[55]); client.println(",");client.println(arr[56]); client.println(",");client.println(arr[57]); client.println(",");client.println(arr[58]); client.println(",");client.println(arr[59]); client.println(",");
            client.println(arr[60]); client.println(",");client.println(arr[61]); client.println(",");client.println(arr[62]); client.println(",");client.println(arr[63]); client.println(",");client.println(arr[64]); client.println(",");client.println(arr[65]); client.println(",");client.println(arr[66]); client.println(",");client.println(arr[67]); client.println(",");client.println(arr[68]); client.println(",");client.println(arr[69]); client.println(",");
            client.println(arr[70]); client.println(",");client.println(arr[71]); client.println(",");client.println(arr[72]); client.println(",");client.println(arr[73]); client.println(",");client.println(arr[74]); client.println(",");client.println(arr[75]); client.println(",");client.println(arr[76]); client.println(",");client.println(arr[77]); client.println(",");client.println(arr[78]); client.println(",");client.println(arr[79]); client.println(",");
            client.println(arr[80]); client.println(",");client.println(arr[81]); client.println(",");client.println(arr[82]); client.println(",");client.println(arr[83]); client.println(",");client.println(arr[84]); client.println(",");client.println(arr[85]); client.println(",");client.println(arr[86]); client.println(",");client.println(arr[87]); client.println(",");client.println(arr[88]); client.println(",");client.println(arr[89]); client.println(",");
            //client.println(arr[90]);
            client.println("], }] };   ");
            client.println("const config = { type: 'line', data: data, options: {} }; </script><script> const myChart = new Chart( document.getElementById('myChart'), config ); </script>");
            client.println("</div> <p>Data processing from CO analyzer is presented in the form of a graph.</p> <p>The X axis - represents the unit of time</p> <p>The Y axis - CO level in units of measurement (ppm)</p> </div> <div class=\"card\"> <h2>Tabular presentation of dimension data</h2>");
            client.println("<table> <tr> <th>Show signal sensor, (ppm) </th> <th>Difference, (ppm)</th> <th>Time of measurements, (c)</th></tr>");
            client.println("<tr> <td>");client.println(arr[0]);client.println("</td> <td>"); client.println(arrayDiff[0]);client.println("</td> <td>0</td> </tr>");
            client.println("<tr> <td>");client.println(arr[1]);client.println("</td> <td>"); client.println(arrayDiff[1]);client.println("</td> <td>1</td> </tr>");
            client.println("<tr> <td>");client.println(arr[2]);client.println("</td> <td>"); client.println(arrayDiff[2]);client.println("</td> <td>2</td> </tr>");
            client.println("<tr> <td>");client.println(arr[3]);client.println("</td> <td>"); client.println(arrayDiff[3]);client.println("</td> <td>3</td> </tr>");
            client.println("<tr> <td>");client.println(arr[4]);client.println("</td> <td>"); client.println(arrayDiff[4]);client.println("</td> <td>4</td> </tr>");
            client.println("<tr> <td>");client.println(arr[5]);client.println("</td> <td>"); client.println(arrayDiff[5]);client.println("</td> <td>5</td> </tr>");
            client.println("<tr> <td>");client.println(arr[6]);client.println("</td> <td>"); client.println(arrayDiff[6]);client.println("</td> <td>6</td> </tr>");
            client.println("<tr> <td>");client.println(arr[7]);client.println("</td> <td>"); client.println(arrayDiff[7]);client.println("</td> <td>7</td> </tr>");
            client.println("<tr> <td>");client.println(arr[8]);client.println("</td> <td>"); client.println(arrayDiff[8]);client.println("</td> <td>8</td> </tr>");
            client.println("<tr> <td>");client.println(arr[9]);client.println("</td> <td>"); client.println(arrayDiff[9]);client.println("</td> <td>9</td> </tr>");
            client.println("<tr> <td>");client.println(arr[10]);client.println("</td> <td>"); client.println(arrayDiff[10]);client.println("</td> <td>10</td>");
            client.println("</tr></table>");

            client.println("</div> </div> <div class=\"rightcolumn\"> <div class=\"card\"> <h2>CO Analyzer</h2> <div class=\"fakeimg\" style=\"height:100px;\"> <p>Main components : Microcontroller ESP-32 , CO sensor MQ-7, Line stabilizer L1117 33C, MOSFET transistor IRLZ44 12</p> </div> </div> <div class=\"card\"> <h3>ESP-WROOM-32</h3> <div class=\"fakeimg0\"><img style=\"width: 100%\" src=\"https://raspberry.com.ua/data/uploads/2020/05/3269-13.jpg\" /></div> </div> <div class=\"card\"> <h3>Download data in pdf</h3> <div class=\"fakeimg\"><button class=\"button\">load</button></div> </div> </div> </div> <div class=\"footer\"> <h3>CO Analyzer</h3> <h3>by Igor Yashan , ILiaya</h3> </div>");   
            client.println("</body></html>");
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c; 
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
   }

}
