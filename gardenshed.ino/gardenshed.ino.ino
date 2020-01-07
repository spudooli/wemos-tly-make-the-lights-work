#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WEMOS_Matrix_LED.h>

#include <WEMOS_SHT3X.h>

SHT3X sht30(0x45);


ESP8266WebServer server(80);

IPAddress ip(192, 168, 2, 47);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(1, 1, 1, 1);
const char* ssid     = "www.spudooli.com_IoT";
const char* password = "F1shfood";

MLED mled(1); //set intensity=5


const int ledPin = D4;

void conditions() {
  Serial.println("conditions");
  if(sht30.get()==0){
    Serial.println(sht30.humidity);
    Serial.println(sht30.cTemp);
    String message = "";
    message += String(sht30.humidity) + ",";
    message += sht30.cTemp;
    server.send(200, "text/html", message);
  }
}

void makeWIFI(){
    //WiFi.config(ip, gateway, subnet, DNS);
  delay(100);
  //WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Fail connecting");
    delay(5000);
    ESP.restart();
  }
  Serial.print("   OK  ");
  Serial.print("Module IP: ");
  Serial.println(WiFi.localIP());
  
  //Turn on the LED because we are WIFI connected
  digitalWrite(ledPin, LOW);
  ledPanelOn();
  
  server.on("/conditions", conditions);
  
  server.begin();
  Serial.println("HTTP server started");
}

void ledPanelOn(){
   for(int y=0;y<8;y++)
  {
    for(int x=0;x<8;x++)
    {
        mled.dot(x,y); // draw dot
        mled.display();
        delay(20);     
    }  
  }
}

void ledPanelOff(){
   for(int y=0;y<8;y++)
  {
    for(int x=0;x<8;x++)
    {
        mled.dot(x,y,0); // turn off the LED dot
        mled.display();
        delay(20);
    }  
  }
}

void setup(){

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  ledPanelOff();
  Serial.begin(115200);

  makeWIFI();

}

void loop() {
     if (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(ledPin, LOW);
      makeWIFI();
    } else {
        server.handleClient();
    }

    
}

