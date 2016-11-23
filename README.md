# ESPEasy
Easy MultiSensor device based on ESP8266

This is where development takes place. Beware that latest versions may be unstable.

Stable versions including libraries are currently on SoureForge:

http://sourceforge.net/projects/espeasy/

Wiki: http://www.esp8266.nu
Forum: http://www.esp8266.nu/forum

# Introduction
ESPEasy is firmware for ESP8266 modules that is pre-programmed with software support for a number of sensors. Simply attach sensors and configure then on the web interface. Supported sensors list: http://www.letscontrolit.com/wiki/index.php/ESPEasy#Applications and more unofficially supported sensors: https://github.com/ESP8266nu/ESPEasyPluginPlayground however these you need to add manually and recompile the firmware.

# Instructions

 1. Get firmware or compile your own. Follow original instructions for flashing: http://www.letscontrolit.com/wiki/index.php/Tutorial_ESPEasy_Firmware_Upload for nodewatcher support download firmware from this repository.
 2. Upload firmware and wait for the device to boot, may take a few minutes. You will see WiFi network ESP_0 appear and log-in with 'configesp' password.
 3. Once connected go to http://192.168.4.1
 4. Configure WiFi network, for example choose 'open.wlan-si.net' and enter password, for an open network enter any password.
 5. Now the device will connect to the network and let you know the new device IP.
 6. Connect you computer to the network the device is connected to and go the its IP.
 
## Register the node with nodewatcher
1. TODO instructions
 
## Configuring for use with nodewatcher
  1. Go to 'config' and enter the following:
   1. Enter 'Name' to be the same as on nodewatcher
   1. Pick an 'Admin password'
   1. Select 'Protocol' to be Nodewatcher JSON HTTP
   1. Set 'Locate Controller' to push.nodes.wlan-si.net or whatever your server is
   1. Set 'Controller Password' to your sensor UUID as determined on nodewtcher.
   1. Set 'Sensor Delay' to how often do you want the node to wake up if put in Sleep mode. Note you need to conenct two pins of ESP together for Sleep mode to work.
   1. Click Submit
## Configuring sensors
 1. Go to 'Devices' tab
 1. Click on 'Edit' to add a new device
 1. Pick 'Device', it may be hardware or software feature.
  1. Configure WiFi signal reporting by selecting 'Device' to be 'System Info'
  1. Pick 'Name' to be rssi, note all devices configured must have different name+IDX combination, all sensors with same name will be groupe on a graph for nodewatcher.
  1. Set 'Delay' to show how often a variable will be reported. 300 is a good value.
  1. Set 'IDX / Var' to be non-zero
  1. Set 'Indicator' to be 'WiFi RSSI'
  1. Set 'Value Name 1' to what data nama on teh graph will be, follow by comma and unit, this case: 'WiFi signal,dBm'
  1. Click Submit
  1. Click on Devices in top menu to see the value
 1. Repeat the process for diferent sensors, for example with DS18B20.
  1. Configure Temperature by selecting 'Device' to be 'Temperature - DS18b20'
  1. Pick 'Name' to be 'temp_a', note all devices configured must have different name+IDX combination, all sensors with same name will be groupe on a graph for nodewatcher.
  1. Set 'Delay' to show how often a variable will be reported. 300 is a good value.
  1. Set 'IDX / Var' to be non-zero, in this case 1
  1. Set '1st GPIO' to be pin which you have the sensor conencted to
  1. Set 'Value Name 1' to what data nama on the graph will be, follow by comma and unit, this case: 'YourTempName,C'
  1. Click Submit
  1. Now the 'Device Nr' will show one or more ROM IDs if connection to sensors works. Pick first one.
  1. Click Submit
 1. Go to 'Device' menu and repeat the process for next sensor
  1. Pick different IDX then in the first case
  1. Click Submit
  1. Now the 'Device Nr' will show one or more ROM IDs if connection to sensors works. Pick second one.
  1. Click Submit
 
   
