#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <WEMOS_SHT3X.h>

SHT3X sht30(0x45);


ESP8266WebServer server(80);

IPAddress ip(192, 168, 1, 48);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(192, 168, 1, 254);
const char* ssid     = "www.spudooli.com";
const char* password = "F1shfood";

int dooropencount = 0;

const int REED_PIN = D3; // Pin connected to reed switch

bool ignore = false;

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

void fridgedoor() {
  Serial.println("fridgedoor");
    String message = "";
    message += dooropencount;
    server.send(200, "text/html", message);
    dooropencount = 0;
}

void makeWIFI(){
    WiFi.config(ip, gateway, subnet, DNS);
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
  
  server.on("/conditions", conditions);
  server.on("/fridgedoor", fridgedoor);
  
  server.begin();
  Serial.println("HTTP server started");
}



void setup(){

  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
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

int fridgedoorstatus = digitalRead(REED_PIN); // Read the state of the switch
  
  if (fridgedoorstatus == LOW) // If the pin reads low, the door is open.
  {
    if (ignore == false){
      ignore = true;
      Serial.println("open");
      dooropencount = dooropencount + 1;
      Serial.println("Door open count = ");
      Serial.println(dooropencount);
    }
  }
  else
  {
    Serial.println("closed");
    ignore = false;
  }
      delay(500);
}

