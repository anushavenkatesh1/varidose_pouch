#include <EEPROM.h>
#include "BluetoothSerial.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include <Arduino.h>  
#include <Wire.h>
#include <RTClib.h>
           
BluetoothSerial SerialBT;            // Bluetooth Serial object
RTC_DS3231 rtc;                     
  
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int sensorPin = 13;          // touch sensor 
const int ledPin = 27;           //  LED pin blue
const int ledPin1 = 14;         //  LED pin green
const int ledPin2 = 12;         //  LED pin BLUE
const int eepromAddress = 0;   // EEPROM address to store the value

XT_Wav_Class ForceWithYou(Force);   // create an object of type XT_Wav_Class that is used by
                                   // the dac audio class (below), passing wav data as parameter.

XT_DAC_Audio_Class DacAudio(25, 0);  // Create the main player class object.
                                    // Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter = 0;       // Just a counter to use in the serial monitor variables will change
int buttonState = 0;           // variable for reading the pushbutton status

#define lightsensor  36        // ESP32 pin GPIO36 (ADC0) connected to light sensor
#define analogthreshold  600   //threshold for light intensity

void setup() 
{ 
 Serial.begin(115200);
 SerialBT.begin("ESP32");    // Initialize Bluetooth with the device name "ESP32"
 pinMode(ledPin,OUTPUT);
 pinMode(ledPin1,OUTPUT);
 pinMode(ledPin2,OUTPUT);
 pinMode(sensorPin, INPUT);
 int storedValue = EEPROM.read(eepromAddress);
 //Serial.print("Stored Value: ");
 //Serial.println(storedValue);
 
 if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Uncomment the following line to set the RTC to the current date and time
  rtc.adjust(DateTime(__DATE__, __TIME__));
 }

void loop() 
{ 
 digitalWrite(ledPin, HIGH);    // setting the rgb off initially , rgb common anode
 digitalWrite(ledPin1,HIGH);
 digitalWrite(ledPin2, HIGH); 
 
 DateTime now = rtc.now();
 int analogValue = analogRead(lightsensor);   // read the value on analog pin
 //Serial.println(analogValue);
 buttonState = digitalRead(sensorPin);  
 
 if (now.minute() % 2 == 0 && analogValue < analogthreshold && buttonState == LOW) 
 {
  int valueToStore = 42;
  EEPROM.write(eepromAddress, valueToStore);
  EEPROM.commit();
  digitalWrite(ledPin, HIGH);         //blue led is off
  digitalWrite(ledPin1, LOW);         // green led is on  
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(":");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.println(" ");
  Serial.print(now.day(), DEC);
  Serial.print("-");
  Serial.print(now.month(), DEC);
  Serial.print("-");
  Serial.println(now.year(), DEC);
  Serial.println("Alarm ON");      // display on serial monitor
  //delay(1000);
  SerialBT.println("Alarm ON");     // Send a Bluetooth message
 {sound();}
  }

if( analogValue > analogthreshold && buttonState == HIGH ) //|| buttonState == HIGH || analogValue > analogthreshold 
 { 
  digitalWrite(ledPin, LOW);            // blue led is on
  digitalWrite(ledPin1, HIGH);          //green led is off
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print(":");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.println(" ");
  Serial.print(now.day(), DEC);
  Serial.print("-");
  Serial.print(now.month(), DEC);
  Serial.print("-");
  Serial.println(now.year(), DEC);
  Serial.println("Alarm OFF");      // display on serial monitor
  delay(1000);
  SerialBT.println("Alarm OFF");   // Send a Bluetooth message
  delay(59000);
 }
}

void sound()
{
  //delay(20);
  DacAudio.FillBuffer();                // Fill the sound buffer with data
  if (ForceWithYou.Playing == false)    // if not playing,
  DacAudio.Play(&ForceWithYou);         // play it, this will cause it to repeat and repeat...  
}
