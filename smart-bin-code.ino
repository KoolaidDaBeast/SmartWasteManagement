#include "ultrasonic_sensor.h"
#include "sd_card.h"
#include <SoftwareSerial.h>
#include <DHT.h>

//Humidity/Temperature Sensor
#define DHTPIN 8
#define DHTTYPE DHT22
const int HUM_VCC = 9;
const int GREEN_LED = 11;
const int YELLOW_LED = 12; 
const int RED_LED = 13;
const int MAX_HEIGHT = 52.5;

//Bluetooth serial
SoftwareSerial bluetoothSerial(6, 7);

//SD Card consts
File dataFile;
String binStatus = "";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Starting serial for both arduino and bluetooth
  Serial.begin(9600);
  bluetoothSerial.begin(9600);

  //Setting up the sensors and datalogging sheilds
  setup_sensor();
  dht.begin();
  setupSDCard();
  dataFile = createFile("bin-data.csv");
  writeNewLineToFile(dataFile, "stamp,hum,height,status");

  //Setting up the pins
  pinMode(HUM_VCC, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
}

void ledDisplay(double trashLevel) {
  if (trashLevel < 25) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    binStatus = "LOW";
    return;
  }
  else if (trashLevel < 75) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    binStatus = "MIDDLE";
    return;
  }
  else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    binStatus = "HIGH";
    return;
  }
}

void loop() {
  delayLogs();

  //Setting the humdity sensor VCC pin to HIGH
  digitalWrite(HUM_VCC, HIGH);

  float humdity = dht.readHumidity();
  double distance = getDistance();

    //Bluetooth Serial
  bluetoothSerial.println(String(humdity) + " " + String(distance));

  double trashLevel = ((MAX_HEIGHT - distance) / MAX_HEIGHT) * 100;

  if (trashLevel < 25) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    binStatus = "LOW";
  }
  else if (trashLevel > 25 && trashLevel < 75) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    binStatus = "MIDDLE";
  }
  else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    binStatus = "HIGH";
  }

  //Datalogging
  String data = getTimestamp() + "," + String(humdity) + "," + String(trashLevel) + "," + binStatus;

  writeNewLineToFile(dataFile, data);

  Serial.println(data);

  if (isSynced()) return;
}
