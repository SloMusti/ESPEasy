//#######################################################################################################
//########################### Controller Plugin 011: Nodewatcher ########################################
//#######################################################################################################

#define CPLUGIN_011
#define CPLUGIN_ID_011         11
#define CPLUGIN_NAME_011       "Nodewatcher JSON HTTP"

boolean CPlugin_011(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_011;
        Protocol[protocolCount].usesMQTT = false;
        Protocol[protocolCount].usesAccount = false;
        Protocol[protocolCount].usesPassword = true;
        Protocol[protocolCount].defaultPort = 80;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_011);
        break;
      }
      
    case CPLUGIN_PROTOCOL_SEND:
      {
        char log[80];
        boolean success = false;
        char host[20];
        sprintf_P(host, PSTR("%u.%u.%u.%u"), Settings.Controller_IP[0], Settings.Controller_IP[1], Settings.Controller_IP[2], Settings.Controller_IP[3]); // ? not sure what it does

        sprintf_P(log, PSTR("%s%s using port %u"), "HTTP : connecting to ", host,Settings.ControllerPort);
        addLog(LOG_LEVEL_DEBUG, log);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        if (!client.connect(host, Settings.ControllerPort))
        {
          connectionFailures++;
          strcpy_P(log, PSTR("HTTP : connection failed"));
          addLog(LOG_LEVEL_ERROR, log);
          return false;
        }
    
        statusLED(true);
        if (connectionFailures)
          connectionFailures--;

        // start constructing the message

        // preparing JSON data for nodewatcher manually, lighter then libraries
        //header
        String postDataStr = "{\"sensors.generic\": {\"_meta\": {\"version\": 1";
        //################################################################
        // EDIT HERE with your sensor variables, copy/paste for more sensors
        // rssi example of how data is sent 
        // postDataStr +="},\"rssi\": {\"name\": \"WiFi signal strength\", \"unit\": \"dBm\", \"value\":";
        // postDataStr += String(WiFi.RSSI());


        postDataStr += F("},\"");
        postDataStr += ExtraTaskSettings.TaskDeviceName;
        postDataStr += F("_");
        postDataStr += event->idx;;
        postDataStr += F("\": {");
        
        byte valueCount = getValueCountFromSensorType(event->sensorType); // multivalue handling required, figure out json
        for (byte x = 0; x < valueCount; x++)
        {
          //workaround since there is no unit field in the system
          String temp_name = String(ExtraTaskSettings.TaskDeviceValueNames[x]);
          //search for two delimiters
          int commaIndex = temp_name.indexOf(',');
          int commaIndex2 = temp_name.indexOf(',',commaIndex+1);

          postDataStr += F("\"name\": \"");
          postDataStr += temp_name.substring(0, commaIndex);
          postDataStr += F("\", \"unit\": \"");
          //safeguard if unit is not entered
          if(commaIndex>0){
            postDataStr += temp_name.substring(commaIndex+1,commaIndex2);
            //we know unit is defined, now lets check for group
            if(commaIndex2>0){
              postDataStr += F("\", ");
              postDataStr += F("\"group\": \"");
              postDataStr += temp_name.substring(commaIndex2+1);
            }
          }
          postDataStr += F("\", ");
          postDataStr += F("\"value\":");          
          postDataStr += toString(UserVar[event->BaseVarIndex + x],ExtraTaskSettings.TaskDeviceValueDecimals[x]);
          postDataStr += F("}");
          //add a comma except on the last one
          if(x!=valueCount-1){
            postDataStr += F(",");
            }
        }
        postDataStr += F("");

        //################################################################
        //footer
        postDataStr += "}}\r\n";

        String hostName = F("push.nodes.wlan-si.net");
        if (Settings.UseDNS)
          hostName = Settings.ControllerHostName;

        String postStr = F("POST /push/http/");
        postStr += SecuritySettings.ControllerPassword; // used for UUID
        postStr += F(" HTTP/1.1\r\n");
        postStr += F("Host: ");
        postStr += hostName;
        postStr += F("\r\n");
        postStr += F("User-Agent: Arduino/1.0\r\n");
        postStr += F("Content-Type: text/plain\r\n");
        postStr += F("Content-Length: ");
        postStr += postDataStr.length();
        postStr += F("\r\n");
        postStr += F("Connection: close\r\n\r\n");
        postStr += postDataStr;

        // This will send the request to the server
        client.print(postStr);
        Serial.println(postStr);

        unsigned long timer = millis() + 200;
        while (!client.available() && millis() < timer)
          delay(1);

        // Read all the lines of the reply from server and print them to Serial
        while (client.available()) {
          String line = client.readStringUntil('\n');
          line.toCharArray(log, 80);
          addLog(LOG_LEVEL_DEBUG_MORE, log);
          if (line.substring(0, 15) == "HTTP/1.1 200 OK")
          {
            strcpy_P(log, PSTR("HTTP : Succes!"));
            addLog(LOG_LEVEL_DEBUG, log);
            success = true;
          }
          delay(1);
        }
        strcpy_P(log, PSTR("HTTP : closing connection"));
        addLog(LOG_LEVEL_DEBUG, log);

        client.flush();
        client.stop();
        break;
      }

  }
  return success;
}


