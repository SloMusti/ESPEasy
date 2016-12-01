//#######################################################################################################
//#################################### Plugin 130: OLED SSD1306 display #################################
//#######################################################################################################


#define PLUGIN_130
#define PLUGIN_ID_130         130
#define PLUGIN_NAME_130       "Telegram BOT"
#define PLUGIN_VALUENAME1_130 "Telegram"

#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>

byte Plugin_130_OLED_address = 0x3c;
byte Plugin_130_OLED_type = 0;

// Initialize Telegram BOT
#define BOTtoken "244194594:AAH3sDXnhi3sIMvu_MO_3LIi1o1zWa82n18"
#define BOTname "MustiBot"
#define BOTusername "slomustibot"

TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
int Sleep_delay = 20000; //go to sleep after this time
long Sleep_lasttime;   //last time messages' scan has been done
bool Start = false;

boolean Plugin_130(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte displayTimer = 0;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_130;
        Device[deviceCount].Type = DEVICE_TYPE_I2C;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 0;
        Device[deviceCount].SendDataOption = false;
        Device[deviceCount].TimerOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_130);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_130));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        String options[2];
        options[0] = F("3C");
        options[1] = F("3D");
        int optionValues[2];
        optionValues[0] = 0x3C;
        optionValues[1] = 0x3D;
        string += F("<TR><TD>I2C Address:<TD><select name='plugin_130_adr'>");
        for (byte x = 0; x < 2; x++)
        {
          string += F("<option value='");
          string += optionValues[x];
          string += "'";
          if (choice == optionValues[x])
            string += F(" selected");
          string += ">";
          string += options[x];
          string += F("</option>");
        }
        string += F("</select>");

        byte choice2 = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
        String options2[2];
        options2[0] = F("Normal");
        options2[1] = F("Rotated");
        int optionValues2[2];
        optionValues2[0] = 1;
        optionValues2[1] = 2;
        string += F("<TR><TD>Rotation:<TD><select name='plugin_130_rotate'>");
        for (byte x = 0; x < 2; x++)
        {
          string += F("<option value='");
          string += optionValues2[x];
          string += "'";
          if (choice2 == optionValues2[x])
            string += F(" selected");
          string += ">";
          string += options2[x];
          string += F("</option>");
        }
        string += F("</select>");

        byte choice3 = Settings.TaskDevicePluginConfig[event->TaskIndex][3];
        String options3[2];
        options3[0] = F("128x64");
        options3[1] = F("64x48");
        int optionValues3[2];
        optionValues3[0] = 1;
        optionValues3[1] = 2;
        string += F("<TR><TD>Display Size:<TD><select name='plugin_130_size'>");
        for (byte x = 0; x < 2; x++)
        {
          string += F("<option value='");
          string += optionValues3[x];
          string += "'";
          if (choice3 == optionValues3[x])
            string += F(" selected");
          string += ">";
          string += options3[x];
          string += F("</option>");
        }
        string += F("</select>");

        char deviceTemplate[8][64];
        LoadCustomTaskSettings(event->TaskIndex, (byte*)&deviceTemplate, sizeof(deviceTemplate));
        for (byte varNr = 0; varNr < 8; varNr++)
        {
          string += F("<TR><TD>Line ");
          string += varNr + 1;
          string += F(":<TD><input type='text' size='64' maxlength='64' name='Plugin_130_template");
          string += varNr + 1;
          string += F("' value='");
          string += deviceTemplate[varNr];
          string += F("'>");
        }

        string += F("<TR><TD>Display button:<TD>");
        addPinSelect(false, string, "taskdevicepin3", Settings.TaskDevicePin3[event->TaskIndex]);

        char tmpString[128];

        sprintf_P(tmpString, PSTR("<TR><TD>Display Timeout:<TD><input type='text' name='plugin_23_timer' value='%u'>"), Settings.TaskDevicePluginConfig[event->TaskIndex][2]);
        string += tmpString;
        
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg("plugin_130_adr");
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        String plugin2 = WebServer.arg("plugin_130_rotate");
        Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
        String plugin3 = WebServer.arg("plugin_23_timer");
        Settings.TaskDevicePluginConfig[event->TaskIndex][2] = plugin3.toInt();
        String plugin4 = WebServer.arg("plugin_130_size");
        Settings.TaskDevicePluginConfig[event->TaskIndex][3] = plugin4.toInt();

        char deviceTemplate[8][64];
        for (byte varNr = 0; varNr < 8; varNr++)
        {
          char argc[25];
          String arg = F("Plugin_130_template");
          arg += varNr + 1;
          arg.toCharArray(argc, 25);
          String tmpString = WebServer.arg(argc);
          strncpy(deviceTemplate[varNr], tmpString.c_str(), sizeof(deviceTemplate[varNr]));
        }

        Settings.TaskDeviceID[event->TaskIndex] = 1; // temp fix, needs a dummy value

        SaveCustomTaskSettings(event->TaskIndex, (byte*)&deviceTemplate, sizeof(deviceTemplate));
        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        telegram_init();
        Serial.println("Telegram init");
        /*
        Plugin_130_OLED_type = 0;
        Plugin_130_OLED_address = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        //Plugin_130_StartUp_OLED();
        Plugin_130_clear_display();
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][1] == 2)
        {
          Plugin_130_OLED_type |= OLED_rotated;
          Plugin_130_sendcommand(0xA0 | 0x1);      //SEGREMAP   //Rotate screen 180 deg
          Plugin_130_sendcommand(0xC8);            //COMSCANDEC  Rotate screen 180 Deg
        }
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][3] == 2)
        {
          Plugin_130_OLED_type |= OLED_64x48;
        }

        Plugin_130_sendStrXY("ESP Easy ", 0, 0); // text sent here
        
        if (Settings.TaskDevicePin3[event->TaskIndex] != -1)
          pinMode(Settings.TaskDevicePin3[event->TaskIndex], INPUT_PULLUP);
          */
        displayTimer = Settings.TaskDevicePluginConfig[event->TaskIndex][2];
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        if (Settings.TaskDevicePin3[event->TaskIndex] != -1)
        {
          if (!digitalRead(Settings.TaskDevicePin3[event->TaskIndex]))
          {
            //Plugin_130_displayOn();
            displayTimer = Settings.TaskDevicePluginConfig[event->TaskIndex][2];
          }
        }
        break;
      }

    case PLUGIN_ONCE_A_SECOND:
      {
        if ( displayTimer > 0)
        {
          displayTimer--;
          if (displayTimer == 0){
            String hello = "I am alive, talk to me.";
            //bot.sendMessage(String(269236767), hello, "");
            Serial.println("getUpdates");
            bot.getUpdates(bot.message[0][1]);   // launch API GetUpdates up to xxx message
            Serial.println("exec message");
            Bot_ExecMessages();   // reply to message with Echo
          }
        }
        break;
      }

    case PLUGIN_READ:
      {
        char deviceTemplate[8][64];
        LoadCustomTaskSettings(event->TaskIndex, (byte*)&deviceTemplate, sizeof(deviceTemplate));

        for (byte x = 0; x < 8; x++)
        {
          String tmpString = deviceTemplate[x];
          if (tmpString.length())
          {
            String newString = parseTemplate(tmpString, 16);
            //Plugin_130_sendStrXY(newString.c_str(), x, 0);
          }
        }
        success = false;
        break;
      }

    case PLUGIN_WRITE:
      {
        String tmpString  = string;
        int argIndex = tmpString.indexOf(',');
        if (argIndex)
          tmpString = tmpString.substring(0, argIndex);
        if (tmpString.equalsIgnoreCase(F("OLED")))
        {
          success = true;
          argIndex = string.lastIndexOf(',');
          tmpString = string.substring(argIndex + 1);
          //Plugin_130_sendStrXY(tmpString.c_str(), event->Par1 - 1, event->Par2 - 1);
        }
        if (tmpString.equalsIgnoreCase(F("OLEDCMD")))
        {
          success = true;
          argIndex = string.lastIndexOf(',');
          tmpString = string.substring(argIndex + 1);
          //if (tmpString.equalsIgnoreCase(F("Off")))
            //Plugin_130_displayOff();
          //else if (tmpString.equalsIgnoreCase(F("On")))
            //Plugin_130_displayOn();
          //else if (tmpString.equalsIgnoreCase(F("Clear")))
            //Plugin_130_clear_display();
        }
        break;
      }

  }
  return success;
}

const char Plugin_130_myFont[][8] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00},
  {0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00},
  {0x00, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, 0x00},
  {0x00, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00},
  {0x00, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x00},
  {0x00, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00, 0x00},
  {0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00},
  {0x00, 0xA0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},
  {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00},
  {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00},
  {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00},
  {0x00, 0x62, 0x51, 0x49, 0x49, 0x46, 0x00, 0x00},
  {0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00},
  {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00},
  {0x00, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00},
  {0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00},
  {0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00},
  {0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
  {0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00},
  {0x00, 0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0xAC, 0x6C, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00},
  {0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00},
  {0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00},
  {0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00},
  {0x00, 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, 0x00},
  {0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00},
  {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
  {0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00},
  {0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00},
  {0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00},
  {0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00},
  {0x00, 0x3E, 0x41, 0x41, 0x51, 0x72, 0x00, 0x00},
  {0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00},
  {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00},
  {0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00},
  {0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00},
  {0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00},
  {0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00},
  {0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00},
  {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
  {0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, 0x00},
  {0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00},
  {0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
  {0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00},
  {0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00},
  {0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00},
  {0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, 0x00},
  {0x00, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00},
  {0x00, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00, 0x00},
  {0x00, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00},
  {0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00},
  {0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00},
  {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00},
  {0x00, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00},
  {0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00},
  {0x00, 0x38, 0x44, 0x44, 0x28, 0x00, 0x00, 0x00},
  {0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00},
  {0x00, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00},
  {0x00, 0x08, 0x7E, 0x09, 0x02, 0x00, 0x00, 0x00},
  {0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, 0x00, 0x00},
  {0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00},
  {0x00, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x80, 0x84, 0x7D, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00},
  {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, 0x00},
  {0x00, 0x7C, 0x08, 0x04, 0x7C, 0x00, 0x00, 0x00},
  {0x00, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00},
  {0x00, 0xFC, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00},
  {0x00, 0x18, 0x24, 0x24, 0xFC, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x7C, 0x08, 0x04, 0x00, 0x00, 0x00},
  {0x00, 0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00},
  {0x00, 0x04, 0x7F, 0x44, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00},
  {0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00},
  {0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, 0x00},
  {0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00},
  {0x00, 0x1C, 0xA0, 0xA0, 0x7C, 0x00, 0x00, 0x00},
  {0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00},
  {0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x02, 0x01, 0x01, 0x02, 0x01, 0x00, 0x00},
  {0x00, 0x02, 0x05, 0x05, 0x02, 0x00, 0x00, 0x00}
};

static void Plugin_130_reset_display(void)
{
  Plugin_130_displayOff();
  Plugin_130_clear_display();
  Plugin_130_displayOn();
}


void Plugin_130_StartUp_OLED()
{
  Plugin_130_init_OLED();
  Plugin_130_reset_display();
  Plugin_130_displayOff();
  Plugin_130_setXY(0, 0);
  Plugin_130_clear_display();
  Plugin_130_displayOn();
}


void Plugin_130_displayOn(void)
{
  Plugin_130_sendcommand(0xaf);        //display on
}


void Plugin_130_displayOff(void)
{
  Plugin_130_sendcommand(0xae);    //display off
}


static void Plugin_130_clear_display(void)
{
  unsigned char i, k;
  for (k = 0; k < 8; k++)
  {
    Plugin_130_setXY(k, 0);
    {
      for (i = 0; i < 128; i++) //clear all COL
      {
        Plugin_130_SendChar(0);         //clear all COL
      }
    }
  }
}


// Actually this sends a byte, not a char to draw in the display.
static void Plugin_130_SendChar(unsigned char data)
{
  Wire.beginTransmission(Plugin_130_OLED_address);  // begin transmitting
  Wire.write(0x40);                      //data mode
  Wire.write(data);
  Wire.endTransmission();              // stop transmitting
}


// Prints a display char (not just a byte) in coordinates X Y,
static void Plugin_130_sendCharXY(unsigned char data, int X, int Y)
{
  //if (interrupt && !doing_menu) return; // Stop printing only if interrupt is call but not in button functions
  Plugin_130_setXY(X, Y);
  Wire.beginTransmission(Plugin_130_OLED_address); // begin transmitting
  Wire.write(0x40);//data mode

  for (int i = 0; i < 8; i++)
    Wire.write(pgm_read_byte(Plugin_130_myFont[data - 0x20] + i));

  Wire.endTransmission();    // stop transmitting
}


static void Plugin_130_sendcommand(unsigned char com)
{
  Wire.beginTransmission(Plugin_130_OLED_address);     //begin transmitting
  Wire.write(0x80);                          //command mode
  Wire.write(com);
  Wire.endTransmission();                    // stop transmitting
}


// Set the cursor position in a 16 COL * 8 ROW map (128x64 pixels)
// or 8 COL * 5 ROW map (64x48 pixels)
static void Plugin_130_setXY(unsigned char row, unsigned char col)
{
  switch (Plugin_130_OLED_type)
  {
    case OLED_64x48:
      col += 4;
      break;
    case OLED_64x48 | OLED_rotated:
      col += 4;
      row += 2;
  }

  Plugin_130_sendcommand(0xb0 + row);              //set page address
  Plugin_130_sendcommand(0x00 + (8 * col & 0x0f)); //set low col address
  Plugin_130_sendcommand(0x10 + ((8 * col >> 4) & 0x0f)); //set high col address
}


// Prints a string regardless the cursor position.
static void Plugin_130_sendStr(unsigned char *string)
{
  unsigned char i = 0;
  while (*string)
  {
    for (i = 0; i < 8; i++)
    {
      Plugin_130_SendChar(pgm_read_byte(Plugin_130_myFont[*string - 0x20] + i));
    }
    *string++;
  }
}


// Prints a string in coordinates X Y, being multiples of 8.
// This means we have 16 COLS (0-15) and 8 ROWS (0-7).
static void Plugin_130_sendStrXY(const char *string, int X, int Y)
{
  Plugin_130_setXY(X, Y);
  unsigned char i = 0;
  while (*string)
  {
    for (i = 0; i < 8; i++)
    {
      Plugin_130_SendChar(pgm_read_byte(Plugin_130_myFont[*string - 0x20] + i));
    }
    *string++;
  }
}


static void Plugin_130_init_OLED(void)
{
  Plugin_130_sendcommand(0xae);                //display off
  Plugin_130_sendcommand(0xa6);                //Set Normal Display (default)
  Plugin_130_sendcommand(0xAE);              //DISPLAYOFF
  Plugin_130_sendcommand(0xD5);              //SETDISPLAYCLOCKDIV
  Plugin_130_sendcommand(0x80);              // the suggested ratio 0x80
  Plugin_130_sendcommand(0xA8);              //SSD1306_SETMULTIPLEX
  Plugin_130_sendcommand(0x3F);
  Plugin_130_sendcommand(0xD3);              //SETDISPLAYOFFSET
  Plugin_130_sendcommand(0x0);               //no offset
  Plugin_130_sendcommand(0x40 | 0x0);        //SETSTARTLINE
  Plugin_130_sendcommand(0x8D);              //CHARGEPUMP
  Plugin_130_sendcommand(0x14);
  Plugin_130_sendcommand(0x20);              //MEMORYMODE
  Plugin_130_sendcommand(0x00);              //0x0 act like ks0108
  Plugin_130_sendcommand(0xA0);
  Plugin_130_sendcommand(0xC0);
  Plugin_130_sendcommand(0xDA);              //0xDA
  Plugin_130_sendcommand(0x12);              //COMSCANDEC
  Plugin_130_sendcommand(0x81);              //SETCONTRAS
  Plugin_130_sendcommand(0xCF);
  Plugin_130_sendcommand(0xd9);              //SETPRECHARGE
  Plugin_130_sendcommand(0xF1);
  Plugin_130_sendcommand(0xDB);              //SETVCOMDETECT
  Plugin_130_sendcommand(0x40);
  Plugin_130_sendcommand(0xA4);              //DISPLAYALLON_RESUME
  Plugin_130_sendcommand(0xA6);              //NORMALDISPLAY

  Plugin_130_clear_display();
  Plugin_130_sendcommand(0x2e);            // stop scroll
  Plugin_130_sendcommand(0x20);            //Set Memory Addressing Mode
  Plugin_130_sendcommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
}



void telegram_init() {
 
  bot.begin();      // launch Bot functionalities
  //String hello = "I am alive, talk to me.";
  //bot.sendMessage(String(269236767), hello, "");
  
}


void Bot_ExecMessages() {
  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length());
    if (bot.message[i][5] == "\/ledon") {
      Serial.println("success");
    }
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}



