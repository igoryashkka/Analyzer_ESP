#include "web_server.h"


WiFiServer server(80);

String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

const char* ssid = "lab-503";
const char* password = "***";



void startServer(int counter,int counterCycle,bool isMesurment,bool isReload){

  
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
            client.println("<style>table { font-family: arial, sans-serif; border-collapse: collapse; width: 100%; } td, th { border: 1px solid #202142; text-align: left; padding: 8px; } tr:nth-child(even) { background-color: rgba(0,0,0,0.4); } * { box-sizing: border-box; } body { font-family: 'Poppins', sans-serif; background-color: rgb(13, 17, 23); color: white; /* background: #f1f1f1; */ } /* Header/Blog Title rgba(0, 0, 0, 0.4) */ .header { padding: 30px; text-align: center; background-color: rgba(0,0,0,0.4); } .header h1 { font-size: 50px; color:rgb(255, 255, 255); } .header p{ color:rgb(255, 255, 255); } .button{ font-family: 'Poppins', sans-serif; font-size: 25px; color: white; width: 100%; height: 100%;background-color: rgb(13, 17, 23); border: none; } .button:hover{ color :rgb(69, 100, 100); } .leftcolumn { float: left; width: 75%;background-color: rgb(13, 17, 23);} .rightcolumn { float: left; width: 25%; background-color: rgb(13, 17, 23);} .fakeimg0 { background-color: rgb(13, 17, 23); width: 100%; } .fakeimg { background-color: black; width: 100%; padding: 20px; } /* Add a card effect for articles */ .card { background-color: rgb(13, 17, 23); padding: 20px; margin-top: 20px; } .row:after { content: \"\"; display: table; clear: both; } /* Footer */ .footer { padding: 20px; text-align: center; background-color: rgba(0,0,0,0.4); margin-top: 5px; color:rgb(255, 255, 255); } /* Responsive layout - when the screen is less than 800px wide, make the two columns stack on top of each other instead of next to each other */ @media screen and (max-width: 800px) { .leftcolumn, .rightcolumn { width: 100%; padding: 0; } } /* Responsive layout - when the screen is less than 400px wide, make the navigation links stack on top of each other instead of next to each other */ @media screen and (max-width: 400px) { .topnav a { float: none; width: 100%; } } ");
            client.println("</style></head>");
            client.println("");

            client.println("<body><div class=\"header\"> <h1>CO Analyzer</h1> <p>Built on ESP32</p> </div>");
            client.println("<div class=\"row\"> <div class=\"leftcolumn\"> <div class=\"card\"> <h2>Data processing from CO analyzer is presented in the form of a graph.</h2> <h5> The measurement cycle is 90 seconds. ");
                client.println("Number of cycle");
            client.println(counterCycle);
            client.println(" Seconds : ");
            client.println(counter);

          if(!isMesurment){
            
               client.println(" Pls WAIT 60 sec to reload page ...");

            }

            
             if(isReload){
             client.println("<script> window.setTimeout( function(){ window.location.reload();}, 5000); </script>");
            }
            
            
            client.println("</h5> <div class=\"fakeimg\" style=\"height:800px;\">");
            client.println("<div> <canvas id=\"myChart\"></canvas> </div> <script> const labels = [ '00:01','00:02','00:03','00:04','00:05','00:06','00:07','00:08','00:09','00:10','00:11','00:12','00:13','00:14','00:15','00:16','00:17','00:18','00:19','00:20','00:21','00:22','00:23','00:24','00:25','00:26','00:27','00:28','00:29','00:30','00:31','00:32','00:33','00:34','00:35','00:36','00:37','00:38','00:39','00:40','00:41','00:42','00:43','00:44','00:45','00:46','00:47','00:48','00:49','00:50','00:51','00:52','00:53','00:54','00:55','00:56','00:57','00:58','00:59','01:00','01:01','01:02','01:03','01:04','01:05','01:06','01:07','01:08','01:09','01:10','01:11','01:12','01:13','01:14','01:15','01:16','01:17','01:18','01:19','01:20','01:21','01:22','01:23','01:24','01:25','01:26','01:27','01:28','01:29','01:30', ]; const data = { labels: labels, datasets: [{ label: 'My First dataset', backgroundColor: 'rgb(255, 99, 132)', borderColor: 'rgb(255, 99, 132)',");
            client.println("  data: [");
            for(int i = 0;i < 90;i++){
              client.println(arr[i]); client.println(",");
              }
            client.println("], }] };   ");
            client.println("const config = { type: 'line', data: data, options: {} }; </script><script> const myChart = new Chart( document.getElementById('myChart'), config ); </script>");
            client.println("</div> <p>Data processing from CO analyzer is presented in the form of a graph.</p> <p>The X axis - represents the unit of time</p> <p>The Y axis - CO level in units of measurement (ppm)</p> </div> <div class=\"card\"> <h2>Tabular presentation of dimension data</h2>");
            client.println("<table> <tr> <th>Show signal sensor, (ppm) </th> <th>Difference, (ppm)</th> <th>Time of measurements, (c)</th></tr>");
              for(int i = 0, jTime = 1;i < 90;i++,jTime++){
            client.println("<tr> <td>");client.println(arr[i]);client.println("</td> <td>"); client.println(arrayDiff[i]);client.println("</td> <td>");client.println(jTime);client.println("</td> </tr>");

            }
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