/* Dieser Arduino-Sketch misst den Abstand von überholenden/vorbeifahrenden Autofahrern an Fahrradfahrern.
 * Befestige den Arduino mit Datalogging-Shield und HC-SR04 Ultraschallsensor an deinem Fahrrad, sodass dieser nach links...
 * ...zur Fahrbahnmitte zeigt.
 * Er misst den Durchschnittlichen Abstand und die Zeitpunkte wenn Auto Sensorbereich betritt und verlässt.
 * Programmiert von Matthias Musch für meinen Vater der täglich mit dem Fahrrad zur Arbeit fährt
 * 14.05.2021
 */

#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
 
RTC_DS1307 RTC;

// Ultraschall Sensor HC-SR04
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 // attach pin D3 Arduino to pin Trig of HC-SR04

//int distance; // variable for the distance measurement

// Zyklischer Puffer für die gemessenen Abstandswerte
//#define DISTANCE_BUF_SIZE 10
//int distanceBuf[DISTANCE_BUF_SIZE];
//int index = 0;

#define NUM_MEASSUREMENTS 5
int sumDistances; // variable for the duration of sound wave travel

File myFile;
 
void setup () 
{
  Serial.begin(57600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) 
  {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  delay(1000);
    
  // Echtzeituhr
  Wire.begin();
  RTC.begin();

  // Konfiguration der Ultraschall-Sensor Pins
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);
 
  /*if (! RTC.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    //sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }*/
 
}

// Idk if this should be implemented, for now file stays open
/*void shutoff() 
{
  myFile.close();
  Serial.println("done.");
}*/
 
void loop () 
{
  // Make some measurements
  for (int i = 0; i < NUM_MEASSUREMENTS; i++) 
  {
    // --- MESSE DEN ABSTAND ---
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    long duration = pulseIn(echoPin, HIGH);
    
    sumDistances += duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  }

  // Take the average of the measurements
  float avg = float(sumDistances) / float(NUM_MEASSUREMENTS);

  // Check if avg is between some boundries indicating that a car is passing the sensor
  if (avg < 170.0f) // [cm]
  {
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("msmt.txt", FILE_WRITE);

    if (myFile) 
    {
      //Serial.print("Writing to msmt.txt...");
      DateTime now = RTC.now();
      myFile.print("Um: ");
      myFile.print(now.day(), DEC);
      myFile.print('/');
      myFile.print(now.month(), DEC);
      myFile.print('/');
      myFile.print(now.year(), DEC);
      myFile.print(' ');
      myFile.print(now.hour(), DEC);
      myFile.print(':');
      myFile.print(now.minute(), DEC);
      myFile.print(':');
      myFile.print(now.second(), DEC);
      myFile.print(" Abstand: ");
      myFile.print(avg);
      myFile.print("\n");

      //Close file
      myFile.close();
    } 
    else 
    {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
    
  }
  
  // Reset global stuff:
  sumDistances = 0;

  
    /*DateTime now = RTC.now();  
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();*/
    delay(100);
}
