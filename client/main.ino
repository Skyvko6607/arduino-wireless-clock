#include <TimeLib.h>
#include "SevSeg.h"
#include <SoftwareSerial.h>

#define SET_TIME_CMD ">settime"
#define NANO_PREFIX "[Nano] >> "
#define NODE_PREFIX "[NodeMCU] >> "

const String set_time_cmd = SET_TIME_CMD;

const byte powerLed[] = {2, 3, 4, 5};
const byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12};

unsigned long elapsed = 0;
unsigned long currentMillis = 0;
const long interval = 1000;

unsigned long clockTime = (long)1592701200000;

SevSeg sevseg;
SoftwareSerial esp8266(A0, A1);

void setup()
{
  // setupOutput(powerLed);
  // setupOutput(segmentPins);

  byte numDigits = 4;
  bool resistorsOnSegments = false;   // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false;      // Default 'false' is Recommended
  bool leadingZeros = false;          // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true;        // Use 'true' if your decimal point doesn't exist or isn't connected

  sevseg.begin(hardwareConfig, numDigits, powerLed, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
  Serial.begin(115200);
  esp8266.begin(57600);

  delay(1000);
  esp8266.write(">gettime");
  adjustTime(clockTime);
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

void loop()
{
  if (esp8266.available() > 0)
  {
    String received = esp8266.readString();
    String cmd = getValue(received, ' ', 0);
    if (cmd == set_time_cmd)
    {
      String argument = getValue(received, ' ', 1);
      clockTime = argument.toInt();
      adjustTime(clockTime);
      printlnNano("Idő frissítve!");
    }
    else
    {
      printlnMCU(received);
    }
  }
  if (Serial.available() > 0)
  {
    String received = Serial.readString();
    if (received[0] == '>')
    {
      esp8266.write(received.c_str());
      printlnNano("Parancs elküldve!");
    }
    // printlnNano(received.c_str());
  }

  currentMillis = millis();
  if (currentMillis - elapsed >= interval)
  {
    elapsed = currentMillis;
    esp8266.write(">gettime");
    setLEDTime();
  }
  sevseg.refreshDisplay();
  delay(50);
}

void printlnNano(String s)
{
  Serial.println(NANO_PREFIX + s);
}

void printlnMCU(String s)
{
  Serial.println(NODE_PREFIX + s);
}

void setLEDTime()
{
  const String textTime = getFormatedTime(hour()) + getFormatedTime(minute());
  const int t = textTime.toInt();
  sevseg.setNumber(t, 0);
}

// void setupOutput(const int pins[])
// {
//   for (int i = 0; i < sizeof(pins); i++)
//   {
//     pinMode(pins[i], OUTPUT);
//   }
// }

String getFormatedTime(int val)
{
  String text = String(val);
  if (text.length() == 1)
  {
    text[1] = text[0];
    text[0] = '0';
  }
  return text;
}