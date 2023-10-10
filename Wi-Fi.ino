#include <Arduino.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "WiFi.h"
#include "Audio.h"

#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

Audio audio;
RTC_DS3231 rtc;

const int ledPin = 2;
const int sensor = 5;
const int eepromAddress = 0; // EEPROM address to store the value

bool audioOn = false; // Flag to track audio status

void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin("Manyatha", "Sriasha@1609");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1500);
    Serial.println("Wi-Fi connection failed. Retrying...");
  }

  Serial.println("Wi-Fi connected!");

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);

  pinMode(sensor, INPUT);
  pinMode(ledPin, OUTPUT);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  int storedValue = EEPROM.read(eepromAddress);
  Serial.print("Stored Value: ");
  Serial.println(storedValue);
}

void loop() {
  audio.loop();
  DateTime now = rtc.now();
  int minutes = now.minute();
  
  int touchState = digitalRead(sensor);

  if (touchState == LOW) {
    // Sensor reading is low
    if (minutes % 10 == 0) {
    if (!audioOn) {
      // Turn on audio if it's not already on
      Serial.println("Alarm on");    // Display on serial monitor
      digitalWrite(ledPin, HIGH);
      audio.connecttospeech("take one Dose of dolo", "en"); // Play audio
      audioOn = true;
       delay(1000);
      int valueToStore = 42;
      EEPROM.write(eepromAddress, valueToStore);
      EEPROM.commit();
    }
  }
  }
  else {
    // Sensor reading is high
    if (audioOn) {
      // Turn off audio if it's currently on
      Serial.println("Alarm off");   // Display on serial monitor
      digitalWrite(ledPin, LOW);
      audio.stopSong(); // Stop audio
      audioOn = false;
      delay(599000);
    }
  }
}
