/*
  [IoT Project: Sensing My Room Temperature]
  DS18B20 senses temperature and ESP8266 transmits data to thingspeak server using WiFi.
  The device also transmits data to my own Database at AWS EC2 server.
  Creator: Raphael Cho, senior of CS department, Sogang University.
  Supervisor: Kyuho Kim, professor of CS department, Sogang University.
  Creation Date: 2019-03-17-Sunday.
  Update Date: 2019-03-24-Sunday.
  Thanks for looking into my code! Have a nice day :D
*/

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D7  // DS18B20 is connected with port D7 on physical board.

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

const char* server = "api.thingspeak.com";
String apiKey = "my_apikey";
const char* my_ssid = "my_ssid"; 
const char* my_pwd = "my_pwd";
int count = 0;

const char* host = "ec2-15-164-49-53.ap-northeast-2.compute.amazonaws.com";
String tail = "/log?";
int seq = 0;

void setup()
{
  Serial.begin(115200);
  connectWiFi();
}

void loop()
{
  float temp; // reserved for temperature datum.

  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);  // gets temperature datum.

  Serial.print(String(count)+" Temperature: ");
  Serial.println(temp); // prints collected datum to serial monitor.
    
  sendTemperature(temp); // sends datum to ThingSpeak.

  delay(59270); // sets transmission period to 1 minute.(why 59.27 sec? -> fix an error!)
}

void connectWiFi()
{
  Serial.print("Connecting to " + *my_ssid);
  WiFi.begin(my_ssid, my_pwd);  // begins WiFi connection.
  
  while (WiFi.status() != WL_CONNECTED) { // if not connected.
    delay(1000);
    Serial.print(".");
  } // exception of unconnected case.
  
  Serial.println("");
  Serial.println("Connected");
  Serial.println("");  
} //end of connect.

void sendTemperature(float temp)
{  
  WiFiClient client;
     
  if (client.connect(server, 80)) { // uses ip [184.106.153.149] or [api.thingspeak.com].
    Serial.println("WiFi Client connected ");

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "\r\n\r\n";  // concatenates string.

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  } //end of if.

  if (client.connect(host, 8080)) {
    Serial.println("WiFi Client connected to DB ");
    
    String data = "seq=" + String(seq) + "&device=1&unit=1&type=T&value=" + String(temp);
    String packet = "GET " + String(tail) + String(data) + " HTTP/1.1";
    
    client.println(packet);
    client.println("User-Agent: rafael");
    client.println("Host: " + String(host));
    client.println("Connection: close");
    client.println();
    
    Serial.println(packet);
  } // end of if for DB.
  
  count++;  // counts number of transmission.
  seq++;
  client.stop();
} //end of send.
