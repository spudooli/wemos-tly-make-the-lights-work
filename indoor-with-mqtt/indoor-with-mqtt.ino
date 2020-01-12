#include <LOLIN_HP303B.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Timer.h"

LOLIN_HP303B HP303B;

#define wifi_ssid "www.spudooli.com_IoT"
#define wifi_password "F1shfood"

#define mqtt_server "192.168.1.2"
#define mqtt_user ""
#define mqtt_password ""

#define pressure_topic "house/indoor/sensor/pressure"
#define temperature_topic "house/indoor/sensor/temperature"


Timer t;

WiFiClient espClientIndoor;
PubSubClient client(espClientIndoor);

const int REED_PIN = D3; // Pin connected to reed switch
const int ledPin = D4;
bool ignore = false;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

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

  HP303B.begin(0x77); // I2C address = 0x77
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266ClientIndoor")) {
      Serial.println("connected");
      //client.publish("house", "indoor/connected", true);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

    int32_t temperature;
    int32_t pressure;
    int16_t oversampling = 7;
    int16_t ret;


String indoortemperature = "";
String indoorpressure = "";

void getConditions() {
  Serial.print("In getConditions");
  ret = HP303B.measureTempOnce(temperature, oversampling);


  indoortemperature = temperature;
  Serial.print(indoortemperature);
  client.publish(temperature_topic, indoortemperature.c_str(), true);

  ret = HP303B.measurePressureOnce(pressure, oversampling);

  indoorpressure =  pressure;
  Serial.print(indoorpressure);
  client.publish(pressure_topic, indoorpressure.c_str(), true);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  t.update();
  
    delay(500);

  }
