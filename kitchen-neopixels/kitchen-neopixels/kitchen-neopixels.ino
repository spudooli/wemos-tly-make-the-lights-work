#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


ESP8266WebServer server(80);

#define PIN 6
const int relayPin = D1;

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(106, D6, NEO_GRB + NEO_KHZ800);

const char* ssid     = "www.spudooli.com_IoT";
const char* password = "F1shfood";

const int ledPin = D4;

void leds_off() {
  Serial.println("leds off");
    String message = "ok";
  server.send(200, "text/html", message);
  for(int x = 0; x < 106; x = x + 1) {         //change the 4 to a 5, 3, or 2 for other patterns
    strip.setPixelColor(x,0,0,0);
    strip.show();
    delay(10);
  }
}

void leds_on() {
  leds_off();
  Serial.println("leds on");
  String message = "ok";
  server.send(200, "text/html", message);
  for(int x = 0; x < 106; x = x + 1) {         //change the 4 to a 5, 3, or 2 for other patterns
    strip.setPixelColor(x,0,0,200);
    strip.show();
    delay(10);
  }
}

void leds_on_half() {
  leds_off();
  Serial.println("leds half");
    String message = "ok";
  server.send(200, "text/html", message);
  for(int x = 0; x < 106; x = x + 2) {         //change the 4 to a 5, 3, or 2 for other patterns
    strip.setPixelColor(x,0,0,200);
    strip.show();
    delay(10);
  }
}

void leds_on_quarter() {
  leds_off();
  Serial.println("leds quarter");
    String message = "ok";
  server.send(200, "text/html", message);
  for(int x = 0; x < 106; x = x + 4) {         //change the 4 to a 5, 3, or 2 for other patterns
    strip.setPixelColor(x,0,0,200);
    strip.show();
    delay(10);
  }
}

void  benchleds_on() {
  String message = "ok";
  Serial.println("Bench on");
  digitalWrite(relayPin, HIGH); 
  server.send(200, "text/html", message);
}

void  benchleds_off() {
  String message = "ok";
  Serial.println("Bench off");
  digitalWrite(relayPin, LOW); 
  server.send(200, "text/html", message);
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
  
  server.on("/leds-on", leds_on);
  server.on("/leds-off", leds_off);
  server.on("/leds-quarter", leds_on_quarter);
  server.on("/leds-half", leds_on_half);

  server.on("/benchleds-on", benchleds_on);
  server.on("/benchleds-off", benchleds_off);
  
  server.begin();
  Serial.println("HTTP server started");
}


void setup(){

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  Serial.begin(115200);
  
  pinMode(D6, OUTPUT);
  
  makeWIFI();

  strip.begin();
  //strip.show();
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

