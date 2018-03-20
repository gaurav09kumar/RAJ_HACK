#include <ESP8266HTTPClient.h>//gps
#include <ArduinoJson.h>//gps
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
//voltage sensor
int analogInput = A0;
float vout = 0.0;
float vin = 0.0;
float R1 = 30000.0; //  
float R2 = 7500.0; // 
int value = 0;
 
String apiKey = "72LEWZSPDNRVGCDB";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "gggg";     // replace with your wifi ssid and wpa2 key
const char *pass =  "gggggggg";
const char* server = "api.thingspeak.com";

const int DHTPin =D6;
#define DHTTYPE DHT11 

//pin where the dht11 is connected
 
DHT dht(DHTPin, DHTTYPE);
float t[7];
float h[7];

WiFiClient Client;


//gps
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
String key = "AIzaSyAIXC0QC3vG9qhT75YrVgERS104QgCnWr8";

int status = WL_IDLE_STATUS;
String jsonString = "{\n";

double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1; 
//gsm
const char* host = "maker.ifttt.com";
const int httpsPort = 443;
String url = "/trigger/esp8266/with/key/hULImGh3qrs6Ojt3aHDOLcOwJvdvpJTId4E7t7ZQPsj";
//us
#define TRIGGER D1
#define ECHO    D2
  float duration=0, distance=0;

 
void setup() 
{
   
       Serial.begin(115200);
       delay(10);
       dht.begin();
       //voltage
       pinMode(analogInput, INPUT);
       //us
       pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
  //gps

  
}
 
void loop() 
{
  
      float h =34.4;
      float t = 28.8;
     delay(100);

      
              
               
                 
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              delay(100);
                   //voltage
      value = analogRead(analogInput);
      vout = (value * 5.0) / 1024.0; // see text
      vin = vout / (R2/(R1+R2)); 
      //gps
      delay(100);
      //us
   digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration)*(0.034/2);
  delay(100);
  

  


   digitalWrite(3,LOW);

      char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");

    if (more_text) {
      // Print out the formatted json...
      Serial.println("{");
      Serial.println("\"homeMobileCountryCode\": 234,");  // this is a real UK MCC
      Serial.println("\"homeMobileNetworkCode\": 27,");   // and a real UK MNC
      Serial.println("\"radioType\": \"gsm\",");          // for gsm
      Serial.println("\"carrier\": \"Vodafone\",");       // associated with Vodafone
      Serial.println("\"cellTowers\": [");                // I'm not reporting any cell towers
      Serial.println("],");
     Serial.println("\"wifiAccessPoints\": [");
      for (int i = 0; i < n; ++i)
      {
        Serial.println("{");
        Serial.print("\"macAddress\" : \"");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.println("\",");
        Serial.print("\"signalStrength\": ");
        Serial.println(WiFi.RSSI(i));
        if (i < n - 1)
        {
          Serial.println("},");
        }
        else
        {
          Serial.println("}");
        }
      }
      Serial.println("]");
      Serial.println("}");
    }
    Serial.println(" ");
  }
  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 234,\n"; // this is a real UK MCC
  jsonString += "\"homeMobileNetworkCode\": 27,\n";  // and a real UK MNC
  jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
  jsonString += "\"carrier\": \"Vodafone\",\n";      // associated with Vodafone
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  //--------------------------------------------------------------------

  Serial.println("");
  WiFiClientSecure client;



  //Connect to the client and make the api call
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + "AIzaSyAIXC0QC3vG9qhT75YrVgERS104QgCnWr8");
  Serial.println(" ");
  if (client.connect(Host, 443)) {
    Serial.println("Connected");
    client.println("POST " + thisPage + key + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }

  //Read and parse all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy   = root["accuracy"];
     latitude= 26.879660;
     longitude= 75.812482;
      Serial.print("latitude:");
      Serial.print(latitude);
      Serial.print("Longitude:");
      Serial.print(longitude);
      delay(500);
    }
  }

  Serial.println("closing connection");
  Serial.println();
  client.stop();

  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);
  Serial.print("Accuracy = ");
  Serial.println(accuracy);


                         if (Client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                        
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(vin);
                             postStr +="&field4=";
                             postStr += String(latitude);
                             postStr +="&field5=";
                             postStr += String(longitude);
                              postStr +="&field6=";
                             postStr += String(distance);
                             postStr += "\r\n\r\n";

                             Serial.println("enetr the dragon");
                              delay(1000);
 
                            Client.print("POST /update HTTP/1.1\n");
                             Client.print("Host: api.thingspeak.com\n");
                             Client.print("Connection: close\n");
                             Client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             Client.print("Content-Type: application/x-www-form-urlencoded\n");
                             Client.print("Content-Length: ");
                             Client.print(postStr.length());
                             Client.print("\n\n");
                             Client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                              Serial.print(" voltage ");
                             Serial.print(vin);
                               Serial.print(" distance ");
                             Serial.print(distance);
                             Serial.println("%. Send to Thingspeak.");
                        }
          Client.stop();
 
          Serial.println("Waiting...");
          
   //gsm
   if(vin > 3.0)
   {       
           WiFiClientSecure Client;

           if (!Client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  Client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  while (Client.connected()) {
    String line = Client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
    String line = Client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  Client.stop();
  }
 
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}




