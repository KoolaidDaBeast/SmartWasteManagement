#ifndef sdCard_h
#define sdCard_h

//Libraries
#include "Arduino.h"
#include "SD.h"
#include <Wire.h>
#include "RTClib.h"

void setupSDCard();

void writeToFile(File file, String text);

void writeNewLineToFile(File file, String text);

File createIncrFile();

File createFile(String filename);

File openFile(String filename);

void closeFile(File file);

String getTimestamp();

uint32_t getMillis();

void delayLogs();

boolean isSynced();

#endif