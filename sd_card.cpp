//Libraries
#include "Arduino.h"
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"
#include "sd_card.h"

uint32_t syncTime = 0;

RTC_PCF8523 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;
const int DELAY_TIME = 2000;

void writeToFile(File file, String text)
{
  file.print(text);
  file.flush();
}

void writeNewLineToFile(File file, String text)
{
  file.println(text);
  file.flush();
}


/**
   The error() function, is just a shortcut for us, we use it when something Really Bad happened.
   For example, if we couldn't write to the SD card or open it.
   It prints out the error to the Serial Monitor, and then sits in a while(1); loop forever, also known as a halt
*/
void error(char const *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while (1);
}

void initSDcard()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

File createIncrFile()
{
  //file name must be in 8.3 format (name length at most 8 characters, follwed by a '.' and then a three character extension.
  char filename[] = "MLOG00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[4] = i / 10 + '0';
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      return SD.open(filename, FILE_WRITE);
    }
  }
}

File createFile(String filename)
{
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      return SD.open(filename, FILE_WRITE);
    }

    return SD.open(filename, FILE_WRITE);
}

File openWriteFile(String filename)
{
    if (SD.exists(filename)) {
      // only open a new file if it doesn't exist
      return SD.open(filename, FILE_WRITE);
    }

    Serial.println("New file was created!");
    return SD.open(filename, FILE_WRITE);
}

void initRTC()
{
  Wire.begin();
  
  if (!RTC.begin()) {
    Serial.println("RTC failed");
  }
}

void closeFile(File file)
{
  file.close();
}

void setupSDCard()
{
  // initialize the SD card
  initSDcard();

  initRTC();
}

String getTimestamp()
{
  DateTime now = RTC.now();

  String result = 
  String(now.unixtime()) + ", " +
  String(now.year()) + "/" +
  String(now.month()) + "/" +
  String(now.day()) + " " + 
  String(now.hour()) + ":" +
  String(now.minute()) + ":" +
  String(now.second());

  return result;
}

uint32_t getMillis()
{
  uint32_t m = millis();
  return m;
}

void delayLogs()
{
  delay((DELAY_TIME - 1) - (millis() % DELAY_TIME));
}

boolean isSynced()
{
  if ((millis() - syncTime) < DELAY_TIME) return true;

  syncTime = millis();

  return false;
}
