#include <Arduino.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT; // Bluetooth Serial object

RTC_DS3231 rtc;

const int ledPin = 2;
const int buzzer = 4;
const int sensor = 5;
const int eepromAddress = 0; // EEPROM address to store the value

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Initialize Bluetooth with the device name "ESP32_BT"

  pinMode(sensor, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  int storedValue = EEPROM.read(eepromAddress);
  Serial.print("Stored Value: ");
  Serial.println(storedValue);
}

void loop() {
  DateTime now = rtc.now();
  int minutes = now.minute();
  int touchState = digitalRead(sensor);

  if (touchState == LOW) {
    if (minutes % 10 == 0) {
      SerialBT.println("Alarm on"); // Send a Bluetooth message
      Serial.println("Alarm on"); // display on serial moniotr
      digitalWrite(ledPin, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(1000);

      // Storing a value in EEPROM
      int valueToStore = 42;
      EEPROM.write(eepromAddress, valueToStore);
      EEPROM.commit();
    }
  } else {
    SerialBT.println("Alarm off"); // Send a Bluetooth message
    Serial.println("Alarm OFF"); // display on serial moniotr
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzer, LOW);
    delay(599000);
  }
}
