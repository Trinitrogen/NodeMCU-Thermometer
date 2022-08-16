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
DHT dht1(DHTPIN1, DHTTYPE);
//DHT dht2(DHTPIN2, DHTTYPE);

//Connect (or reconnect) to MQTT Server
void reconnect() {
    //char* clientId = "NodeMCU-Thermometer";
    String clientId = "NodeMCUClient-";
    clientId += String(random(0xffff), HEX);
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(" as Client ");
    Serial.println(clientId.c_str());
    delay(2000);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user,mqtt_pass)){
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("NodeMCU-Thermometer/Status", "On");
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

  //Begin DHT Sensors
  dht1.begin();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(5000);
  
  float Humid1 = dht1.readHumidity();
  float TempC1 = dht1.readTemperature();
  char msgBuffer[20];

  // Check if any reads on the first sensor failed and exit early (to try again).
    if (isnan(Humid1) || isnan(TempC1)) {
    Serial.println(F("Failed to read from DHT 1 sensor!"));
    client.publish("NodeMCU-Thermometer/Status", "Sensor Read Failed");
    delay(2000);
    return;
  }
  //Send Temperature and Humity of sensor 1 to MQTT

  dtostrf(TempC1, 6, 2, msgBuffer);
  Serial.print("TempC1: ");
  Serial.println(msgBuffer);
  client.publish("NodeMCU-Thermometer/Temp1", msgBuffer);
  dtostrf(Humid1, 6, 2, msgBuffer);
  Serial.print("Humid1: ");
  Serial.println(msgBuffer);
  client.publish("NodeMCU-Thermometer/Humid1", msgBuffer);
  client.publish("NodeMCU-Thermometer/Status", "On");
}
