/*
NodeMCU-Thermometer

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "arduino_secrets.h"


//Configure Global Variables
const char* wifi_ssid = WIFI_SSID;
const char* wifi_pass = WIFI_PASS;
const char* mqtt_server = MQTT_SERVER;
const char* mqtt_user = MQTT_USER;
const char* mqtt_pass = MQTT_PASS;

//Instantiate Objects
WiFiClient wifiClient;
PubSubClient client(wifiClient);

//Connect (or reconnect) to MQTT Server
void reconnect() {
    char* clientId = "NodeMCU-Thermometer";
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(" as Client ");
    Serial.println(clientId);
    delay(5000);
    
    // Attempt to connect
    if (client.connect(clientId, mqtt_user,mqtt_pass)){
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
}

void setup() {
  Serial.begin(115200);
  
  //Connect to WIFI
  for (int i = 0; i <= 5; i++) {
    Serial.println("Booting");
    delay(500);
  }
  Serial.print("Connecting to WIFI Network: ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WIFI connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Configure PubSub Client
  client.setServer(mqtt_server, 1883);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
