# NodeMCU-Thermometer
## NodeMCU based thermometer using MQTT Thermometer and a DHT11 or DHT22 to report to Home Assistant

Built this to monitor the temperature and humitity of my 3D Printer Filament enclosure. Requires a NodeMCU and a DHT11/DHT22 sensor. Connect the sensor to GPIO 4, then enter your WiFi and MQTT server information into the Secrets file.

I put my NodeMCU inside [this case](https://www.printables.com/model/187514-esp8266-nodemcu-v2-case), with the sensor protruding from the bottom. 

For Home Assisstant I added the entitities to my configuration.yaml file with 
```
  sensor:
  - platform: mqtt
    name: "Temp1"
    state_topic: "NodeMCU-Thermometer/Temp1"
    unit_of_measurement: "C"
  - platform: mqtt
    name: "Humidity-1"
    state_topic: "NodeMCU-Thermometer/Humid1"
    unit_of_measurement: "%"
```
Followed by adding the entities to Lovelace
```
  - entity: sensor.temp1
    name: Filament Temp
    icon: mdi:temperature-celsius
  - entity: sensor.humidity_1
    name: Filament Relative Humidity
    icon: mdi:water-percent
```
