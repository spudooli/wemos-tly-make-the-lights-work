#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WEMOS_SHT3X.h>
#include "Timer.h"

#define wifi_ssid "www.spudooli.com_IoT"
#define wifi_password "F1shfood"

#define mqtt_server "192.168.1.2"
#define mqtt_user ""
#define mqtt_password ""

#define humidity_topic "house/outside/sensor/humidity"
#define temperature_topic "house/outside/sensor/temperature"

SHT3X sht30(0x45);

Timer t;

WiFiClient espClientoutside;
PubSubClient client(espClientoutside);

const int ledPin = D4;
bool ignore = false;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

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
    if (client.connect("ESP8266Clientoutside")) {
      Serial.println("connected");
      //client.publish("house", "fridge/connected", true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String outsidetemperature = "";
String outsidehumidity = "";

void getConditions() {
  Serial.print("In getConditions");
  sht30.get();

  outsidetemperature = String(sht30.cTemp);
  Serial.print(outsidetemperature);
  client.publish(temperature_topic, outsidetemperature.c_str(), true);

  outsidehumidity =  String(sht30.humidity);
  Serial.print(outsidehumidity);
  client.publish(humidity_topic, outsidehumidity.c_str(), true);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  t.update();
  
    delay(500);

  }
