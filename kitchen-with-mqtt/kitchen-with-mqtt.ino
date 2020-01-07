#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WEMOS_SHT3X.h>
#include "Timer.h"

#define wifi_ssid "www.spudooli.com_IoT"
#define wifi_password "F1shfood"

#define mqtt_server "192.168.1.2"
#define mqtt_user ""
#define mqtt_password ""

#define humidity_topic "house/kitchen/sensor/humidity"
#define temperature_topic "house/kitchen/sensor/temperature"

SHT3X sht30(0x45);

Timer t;

WiFiClient espClient;
PubSubClient client(espClient);

const int REED_PIN = D3; // Pin connected to reed switch
const int ledPin = D4;
bool ignore = false;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(ledPin, OUTPUT);
  t.every(120000, getConditions);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.publish("house", "fridge/connected", true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String kitchentemperature = "";
String kitchenhumidity = "";
String fridgedooropened = "";

void getConditions() {
  Serial.print("In getConditions");
  sht30.get();

  kitchentemperature = String(sht30.cTemp);
  Serial.print(kitchentemperature);
  client.publish(temperature_topic, kitchentemperature.c_str(), true);

  kitchenhumidity =  String(sht30.humidity);
  Serial.print(kitchenhumidity);
  client.publish(humidity_topic, kitchenhumidity.c_str(), true);

}

void sendFridgeDoor(){
  fridgedooropened = "open";
  client.publish("house/kitchen/sensor/door", fridgedooropened.c_str(), true);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  t.update();
  
  int fridgedoorstatus = digitalRead(REED_PIN); // Read the state of the switch
  if (fridgedoorstatus == HIGH) // If the pin reads low, the door is open.
  {
    if (ignore == false) {
      ignore = true;
      Serial.println("open");
      digitalWrite(ledPin, HIGH);
      sendFridgeDoor();
    }
  }
  else
  {
    Serial.println("closed");
    digitalWrite(ledPin, LOW);
    ignore = false;
  }
  delay(500);

  }
