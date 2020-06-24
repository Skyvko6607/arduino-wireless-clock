#include <ESP8266WiFi.h>
#include <time.h>
#include <EEPROM.h>
#include "NTPClient.h"
#include "WiFiUdp.h"

#define HOST_CMD ">sethostname"
#define PASS_CMD ">setpassword"
#define TIME_CMD ">gettime"
#define CLEAR_CMD ">cleardata"
#define MAX_MESSAGE 30

const String host_cmd = HOST_CMD, pass_cmd = PASS_CMD, time_cmd = TIME_CMD, clear_cmd = CLEAR_CMD;

const long timezone = 2 * 3600;
String ssid, pass;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup()
{
  Serial.begin(57600);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  clearEEPROM();
  EEPROM.begin(MAX_MESSAGE);
  String data = readString(0);
  // EEPROM.get(0, data);
  if (data.length() > 0)
  {
    ssid = getValue(data, ' ', 0);
    if (data.indexOf(" ") > 0)
    {
      pass = getValue(data, ' ', 1);
    }

    if (ssid.length() > 0 && pass.length() > 0)
    {
      connectWifi();
    }
  }
  EEPROM.end();
}

String readString(int address)
{
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(address);
  while (k != '\0' && len < 100) //Read until null character
  {
    k = EEPROM.read(address + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void connectWifi()
{
  WiFi.begin(ssid.c_str(), pass.c_str());
  int tries = 10;
  char msg[MAX_MESSAGE];
  while (WiFi.status() != WL_CONNECTED)
  {
    sprintf(msg, "Csatlakozás a szerverre... (%d)", tries);
    Serial.println(msg);
    tries--;
    delay(1000);
    if (tries <= 0 && WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Sikertelen volt a csaltakozás!");
      delay(1000);
      return;
    }
  }
  Serial.println("Felcsatlakozva Wifi szerverre!");
  Serial.println("IP cím: ");
  Serial.println(WiFi.localIP().toString().c_str());
  timeClient.begin();
  timeClient.setTimeOffset(timezone);
  delay(1000);
}

void clearEEPROM()
{
  EEPROM.begin(EEPROM.length());
  for (int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  delay(100);
}

void writeEEPROMData(String data)
{
  EEPROM.begin(data.length());
  int i = 0;
  for (i = 0; i < data.length(); i++)
  {
    EEPROM.write(i, data[i]);
    delay(100);
    Serial.print(".");
  }
  EEPROM.write(i, '\0');
  EEPROM.commit();
  EEPROM.end();
}

void writeCredentials()
{
  String data;
  // strcpy(data, ssid);
  data += ssid;
  if (pass.length() > 0)
  {
    data += " " + pass;
  }
  int i;
  for (i = 0; i < data.length(); i++)
  {
    if (data[i] == '\n' || data[i] == '\r')
    {
      data.remove(i, data.length() - 1);
      break;
    }
  }
  writeEEPROMData(data);
}

unsigned long elapsed, currentMillis;
const long interval = 2000;

void loop()
{
  currentMillis = millis();
  if (WiFi.status() != WL_CONNECTED && ssid.length() > 0 && pass.length() > 0)
  {
    connectWifi();
  }
  else if (WiFi.status() == WL_CONNECTED && currentMillis - elapsed >= interval)
  {
    timeClient.update();
    elapsed = currentMillis;
  }
  if (Serial.available() > 0)
  {
    String r = Serial.readString();
    int i;
    for (i = 0; i < r.length(); i++)
    {
      if (r[i] == '\n' || r[i] == '\r')
      {
        r.remove(i, r.length() - 1);
        break;
      }
    }
    String cmd = getValue(r, ' ', 0);
    if (cmd == host_cmd || cmd == pass_cmd)
    {
      String argument;
      if (r.indexOf(" ") <= 0)
      {
        Serial.println("Helyes használat:");
        Serial.println("\">sethostname <wifi>\"");
        Serial.println("vagy");
        Serial.println("\">setpassword <jelszó>\"");
        return;
      }
      argument = getValue(r, ' ', 1);
      if (cmd == host_cmd)
      {
        // strcpy(ssid, argument);
        ssid = argument;
      }
      else if (cmd == pass_cmd)
      {
        pass = argument;
      }
      Serial.println("Sikeres beállítás!");
      clearEEPROM();
      writeCredentials();
    }
    else if (cmd == time_cmd)
    {
      timeClient.update();
      delay(100);
      int epoch = timeClient.getEpochTime();
      char msg[MAX_MESSAGE];
      sprintf(msg, ">settime %d", epoch);
      Serial.println(msg);
    }
    else if (cmd == clear_cmd)
    {
      clearEEPROM();
      Serial.println("Adatok törölve!");
    }
  }
  delay(100);
}