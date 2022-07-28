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
DHT dht(DHTPIN, DHTTYPE);

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
      client.publish("NodeMCU-Thermometer/Status", "On");
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

  //Begin DHT Sensor
  dht.begin();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float Humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float TempC = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(Humidity) || isnan(TempC) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, Humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(TempC, Humidity, false);

/*  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(TempC);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F")); */

  char msgBuffer[20];           // make sure this is big enough to hold your string
  dtostrf(TempC, 6, 2, msgBuffer);
  Serial.print("Temperature: ");
  Serial.println(msgBuffer);
  client.publish("NodeMCU-Thermometer/Temp1", msgBuffer);
  dtostrf(Humidity, 6, 2, msgBuffer);
  Serial.print("Humidity: ");
  Serial.println(msgBuffer);
  client.publish("NodeMCU-Thermometer/Humidity", msgBuffer);
  
  delay(5000);
}
