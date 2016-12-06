# ESPEasy
Easy MultiSensor device based on ESP8266. This is a forked repository from main branch, implementing specific features. It should be backwards compatible as it does not introduce many/any changes to the core.

Follow the websites of the main project here:

Wiki: http://www.esp8266.nu
Forum: http://www.esp8266.nu/forum

This fork is optimized for use in community wireless network [wlan slovenija](http://wlan-si.net) and with the platform nodewatcher.

# Introduction
ESPEasy is firmware for ESP8266 modules that is pre-programmed with software support for a number of sensors. Simply attach sensors and configure then on the web interface. Supported [sensors list](http://www.letscontrolit.com/wiki/index.php/ESPEasy#Applications) and more [unofficially supported sensors](https://github.com/ESP8266nu/ESPEasyPluginPlayground), however these you need to add manually and recompile the firmware. Firstly you need to register the sensor with the server to which the information will be pushed and secondly configure the sensor itself.

## Register the sensor with nodewatcher
1. Register and log on [nodewatcher](https://nodes.wlan-si.net/)
 1. Select `Register New Node` under your user account
 1. Turn `Advanced mode` to `ON` on top right
 1. Now select `Disable defaults`
 1. Select the sensor name by entering it into `Name` field
 1. Select `Platform` to be `---------`
 1. Configure location on the map if you wish
 1. Select `Telemetry source` to be `Push from Node`
 1. Add `Identity Mechanisms` field and select `HMAC Signature`
 1. Tick the box `Trusted`
 1. Pick your key and enter it in the field. Note that key as you will need to enter it into `Controller User Account` field on the sensor
 1. Change or use the system generated `Authentication` field, you will need to enter it into `Admin password` field on the sensor
 1. Remove `Router Identifier`
 1. Remove `DNS servers`

## Instructions

 1. Get firmware or compile your own. Follow [original instructions for flashing](http://www.letscontrolit.com/wiki/index.php/Tutorial_ESPEasy_Firmware_Upload) for nodewatcher support download firmware from [this repository.](ESPEasy.ino.nodemcu.bin)
 2. Upload firmware and wait for the device to boot, may take a few minutes. You will see WiFi network ESP_0 appear and log-in with `configesp` password.
 3. Once connected go to http://192.168.4.1
 4. Configure WiFi network, for example choose `open.wlan-si.net` and enter password, for an open network enter any password.
 5. Now the device will connect to the network and let you know the new device IP.
 6. Connect you computer to the network the device is connected to and go the its IP.

## Configuring for use with nodewatcher
  1. Go to `config` and enter the following:
   1. Enter `Name` to be the same as on [nodewatcher](https://nodes.wlan-si.net/)
   1. Enter an `Admin password`, the same as on [nodewatcher](https://nodes.wlan-si.net/)
   1. Select `Protocol` to be `Nodewatcher JSON HTTP`
   1. Set `Locate Controller` to `push.nodes.wlan-si.net` or whatever your server is
   1. Set `Controller User Account` to your sensor UUID as determined on [nodewatcher](https://nodes.wlan-si.net/). That is the set of numbers and letters following `https://nodes.wlan-si.net/node/` in the URL. Note that you do not enter the trailing `/`
   1. Set `Controller Password` to your sensor `HMAC Signature` as determined on [nodewatcher](https://nodes.wlan-si.net/).
   1. Set `Sensor Delay` to how often do you want the node to wake up if put in Sleep mode. Note you need to connect two pins of ESP together for Sleep mode to work.
   1. Click `Submit`

## Configuring sensors
 1. Go to `Devices` tab
 1. Click on `Edit` to add a new device
 1. Pick `Device`, it may be hardware or software feature.
  1. Configure WiFi signal reporting by selecting `Device` to be `System Info`
  1. Pick `Name` to be `rssi`, note all devices configured must have different name.
  1. Set `Delay` to show how often a variable will be reported. 300 is a good value.
  1. Set `IDX / Var` to be non-zero, best `1`
  1. Set `Indicator` to be `WiFi RSSI`
  1. Set `Value Name 1` to what data name on the graph will be, follow by comma and unit, optionally followed by group setting, this case: `RSSI,dBm`. Note that you do not need to define the group, bike in this case.
  1. Click Submit
  1. Click on Devices in top menu to see the value
 1. Repeat the process for different sensors, for example with DS18B20.
  1. Configure Temperature by selecting `Device` to be `Temperature - DS18b20`
  1. Pick `Name` to be `temp_1`, note all devices configured must have a different name.
  1. Set `Delay` to show how often a variable will be reported. 300 is a good value.
  1. Set `IDX / Var` to be non-zero, in this case 1
  1. Set `1st GPIO` to be pin which you have the sensor connected to
  1. Set `Value Name 1` to what data name on the graph will be, follow by comma and unit, followed by group setting, this case: `YourTempName1,C,group_1`. Note that you do not need to define the group, but all measurements that have the same group defined will be put on a single graph.
  1. Click Submit
  1. Now the `Device Nr` will show one or more ROM IDs if connection to sensors works. Pick first one.
  1. Click Submit
 1. Go to `Device` menu and repeat the process for next sensor
  1. Pick different name.
  1. Click Submit
  1. Now the `Device Nr` will show one or more ROM IDs if connection to sensors works. Pick second one and enter `Value Name 1`
  1. Click Submit
