#include <Arduino.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "BluetoothSerial.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"

BluetoothSerial SerialBT; // Bluetooth Serial object

RTC_DS3231 rtc;

const int ledPin = 2;
const int sensor = 5;
const int eepromAddress = 0; // EEPROM address to store the value
int flag = 1;

XT_Wav_Class ForceWithYou(Force);     // create an object of type XT_Wav_Class that is used by
// the dac audio class (below), passing wav data as parameter.

XT_DAC_Audio_Class DacAudio(25, 0);   // Create the main player class object.
// Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter = 0;             // Just a counter to use in the serial monitor

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Initialize Bluetooth with the device name "ESP32_BT"

  pinMode(sensor, INPUT);
  pinMode(ledPin, OUTPUT);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  int storedValue = EEPROM.read(eepromAddress);
  Serial.print("Stored Value: ");
  Serial.println(storedValue);
}

void loop() {
  DateTime now = rtc.now();
  int minutes = now.minute();
  int sec = now.second();
 int touchState = digitalRead(sensor);

   
   // if (minutes % 10 == 0) 
   {
      //SerialBT.println("Alarm on"); // Send a Bluetooth message
      Serial.println(minutes);
      Serial.println(sec);
      Serial.println("Alarm on"); // display on serial moniotr
      digitalWrite(ledPin, HIGH);
      flag = 1;
     // delay(1000);

      // Storing a value in EEPROM
      int valueToStore = 42;
      EEPROM.write(eepromAddress, valueToStore);
      EEPROM.commit();
      if ( digitalRead(sensor) == HIGH) {
        //SerialBT.println("Alarm off"); // Send a Bluetooth message
        Serial.println("Alarm OFF"); // display on serial moniotr
        digitalWrite(ledPin, LOW);
        flag = 0;
       delay(10000);
      }
    }
   
  if (flag == 1) {
    delay(20);
    DacAudio.FillBuffer();                // Fill the sound buffer with data
    if (ForceWithYou.Playing == false)    // if not playing,
      DacAudio.Play(&ForceWithYou);       // play it, this will cause it to repeat and repeat...
   // Serial.println(DemoCounter++);        // Showing that the sound will play as well as your code running here.
    }
  }
