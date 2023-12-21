//ACTIVATE THE ALARM FOR EVERY 2MIN

/* In the code provided below, oled(display), ldr and reed switch have been interfaced with the esp32 , 
 where the alarm gets triggered for every 2 min and at the time of alarm the led and olde (display) gets activated 
 along with audio playing for medication alert, once the action is completed it displays the next alarm timing on the display
*/

#include "BluetoothSerial.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(128, 64, &Wire, -1);

BluetoothSerial SerialBT; // Bluetooth Serial object
RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int ledPin = 14;            //  LED pin green
const int ledPin1 = 27;          //  LED pin blue
int reed_switch = 15;           // reed switch pin
#define lightsensor 36         // ESP32 pin GPIO36 ( VP) connected to light sensor
#define analogthreshold 600   //threshold for light intensity

XT_Wav_Class ForceWithYou(Force); // create an object of type XT_Wav_Class that is used by
                                  // the dac audio class (below), passing wav data as parameter.

XT_DAC_Audio_Class DacAudio(25, 0); // Create the main player class object.
                                   // Use GPIO 25, one of the 2 DAC pins and timer 0

void setup()
{
    Serial.begin(115200);
    SerialBT.begin("ESP32");      // Initialize Bluetooth with the device name "ESP32"
    pinMode(ledPin, OUTPUT);
    pinMode(ledPin1, OUTPUT);
    pinMode(reed_switch, INPUT);
 
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        while (1)
            ;
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    // Uncomment the following line to set the RTC to the current date and time
        rtc.adjust(DateTime(__DATE__, __TIME__));
        
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2.9);
        display.setCursor(15,18);
        display.print("VARIDOSE");
    
        DateTime now = rtc.now();
        
        display.setTextSize(2);
        display.setCursor(8,50);
        display.println(now.day(), DEC);    
        
        display.setTextSize(2);
        display.setCursor(25,50);
        display.println("/");
         
        display.setTextSize(2);
        display.setCursor(40,50);
        display.println(now.month(), DEC);
         
        display.setTextSize(2);
        display.setCursor(65,50);
        display.println("/");
        
        display.setTextSize(2);
        display.setCursor(80,50);
        display.println(now.year(), DEC);
        display.display();
        delay(5000); 
}

void loop()
{
    digitalWrite(ledPin, HIGH);         // setting the rgb off initially , rgb common anode
    digitalWrite(ledPin1, HIGH);

    DateTime now = rtc.now();
    int analogValue = analogRead(lightsensor);    
    int reed_status = digitalRead(reed_switch);

    if (now.minute() % 2 == 0 && reed_status == LOW && analogValue < analogthreshold)
    {
        digitalWrite(ledPin, HIGH);          //blue led is off
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
        Serial.println("Alarm ON");              // display on serial monitor
        SerialBT.println("Alarm ON");           // Send a Bluetooth message
        sound();    
  
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(90,45);
        display.println(now.second(), DEC);
       
        display.setTextSize(2);
        display.setCursor(75,45);
        display.println(":");
         
        display.setTextSize(2);
        display.setCursor(50,45);
        display.println(now.minute(), DEC);
                
        display.setTextSize(2);
        display.setCursor(35,45);
        display.println(":");
         
        display.setTextSize(2);
        display.setCursor(10,45);
        display.println(now.hour(), DEC);
          
        display.setTextSize(2);
        display.setCursor(7,0);
        display.print("Medication");
        display.display(); 
        
        display.setTextSize(2);
        display.setCursor(30,23);
        display.print("Alert");
        display.display(); 
    }
  if (now.minute() % 2 != 0 && reed_status == HIGH && analogValue > analogthreshold)     //|| buttonState == HIGH || analogValue > analogthreshold
    {
        digitalWrite(ledPin, LOW); // blue led is on
        digitalWrite(ledPin1, HIGH); //green led is off
       
        DateTime now = rtc.now();
  
        // Increment the minutes by 2 to display the next alarm
        now = now + TimeSpan(0, 0, 1  ,0);
        
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(90,45);
        display.println("00");
       
        display.setTextSize(2);
        display.setCursor(75,45);
        display.println(":");
         
        display.setTextSize(2);
        display.setCursor(50,45);
        display.println(now.minute(), DEC);
                
        display.setTextSize(2);
        display.setCursor(35,45);
        display.println(":");
         
        display.setTextSize(2);
        display.setCursor(10,45);
        display.println(now.hour(), DEC);
          
        display.setTextSize(2);
        display.setCursor(30,0);
        display.print("Next");
        display.display(); 
        
        display.setTextSize(2);
        display.setCursor(30,23);
        display.print("Alarm");
        display.display(); 
          
        Serial.print(now.hour(), DEC);
        Serial.print(":");
        Serial.print(now.minute(), DEC);
        Serial.print(":");
        Serial.print(now.second(), DEC);
        Serial.print(":");
        Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
        Serial.print(" ");
        Serial.print(now.day(), DEC);
        Serial.print("-");
        Serial.print(now.month(), DEC);
        Serial.print("-");
        Serial.println(now.year(), DEC);
        Serial.println("Alarm OFF");
        Serial.println("NEXT ALARM");               // display on serial monitor
        delay(1000);
        SerialBT.println("Alarm OFF");             // Send a Bluetooth message      
    }    
}
void sound()
{
    DacAudio.FillBuffer();             // Fill the sound buffer with data
    if (ForceWithYou.Playing == false) // if not playing,
      DacAudio.Play(&ForceWithYou);  // play it, this will cause it to repeat and repeat...
}
