#include <EEPROM.h>
#include "BluetoothSerial.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include <Arduino.h>
#include <RTClib.h> 

RTC_DS3231 rtc;

BluetoothSerial SerialBT;   // Bluetooth Serial object

const int sensorPin = 13;   // the number of the pushbutton pin
const int ledPin = 14;    // the number of the LED pin GREEN
const int ledPin1 = 27;    // the number of the LED pin BLUE
const int eepromAddress = 0;  // EEPROM address to store the value

int flag = 1;

XT_Wav_Class ForceWithYou(Force);   // create an object of type XT_Wav_Class that is used by
                   // the dac audio class (below), passing wav data as parameter.

XT_DAC_Audio_Class DacAudio(25, 0);  // Create the main player class object.
// Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter = 0;       // Just a counter to use in the serial monitor

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() 
{ Serial.begin(115200);
 SerialBT.begin("ESP32_BT");    // Initialize Bluetooth with the device name "ESP32_BT"
 pinMode(ledPin,OUTPUT);
 pinMode(ledPin1,OUTPUT);
 pinMode(sensorPin, INPUT);
 int storedValue = EEPROM.read(eepromAddress);
 Serial.print("Stored Value: ");
 Serial.println(storedValue);
  rtc.begin(); // Initialize the RTC
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the RTC to the current date and time
}

void loop() { 
  DateTime now = rtc.now(); // Get the current time from the RTC
  int minutes = now.minute(); // Get the minutes component of the current time

  // For every 10 minutes
  if (minutes % 2 == 0) {
 // read the state of the pushbutton value:
 buttonState = digitalRead(sensorPin);  
 if (buttonState == LOW) 
 { int valueToStore = 42;
  EEPROM.write(eepromAddress, valueToStore);
  EEPROM.commit();
 }
 else { // turn LED off:
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPin1, HIGH);
  SerialBT.println("Alarm OFF");   // Send a Bluetooth message
  Serial.println("Alarm OFF");    // display on serial moniotr
  delay(1000);
  flag = 0;
  }
 if (flag == 1) {
  digitalWrite(ledPin, HIGH);
  digitalWrite(ledPin1, LOW);
  SerialBT.println("Alarm ON");     // Send a Bluetooth message
  Serial.println("Alarm ON");      // display on serial moniotr
  delay(20);
  DacAudio.FillBuffer();         // Fill the sound buffer with data
  if (ForceWithYou.Playing == false)   // if not playing,
  DacAudio.Play(&ForceWithYou);     // play it, this will cause it to repeat and repeat...  
  }
}
}
