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

#define NUM_MEASSUREMENTS 3 // Je mehr Messungen man macht desto genauer aber auch desto träger wird das System und schnelle Objekte (zB. Autos) werden ggf. nicht erkannt
int sumDistances; // Durschnittswert der gemessenen Distanzen ermittelt durch Ultraschall-Messung

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

 // Das braucht man nur um initial die RTC zu setzen
  /*if (! RTC.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    //sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }*/
 
}
 
void loop () 
{
  // Führe Ultraschall-Messungen durch
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

  // Berechne den Durschnittswert aus der Messreihe
  float avg = float(sumDistances) / float(NUM_MEASSUREMENTS);
  // Fürs Debugging:
  Serial.println("Abstand:" + String(avg));

  // Wenn der Durchschnittswert weniger ist als eine gewisser Schwellwert berichte das Vorkommniss (zB überholendes Auto) und schreibe es auf die SD-Karte
  if (avg < 170.0f) // [cm]
  {
    // Öffne die Datei. Beachte: Es kann immer nur eine Datei gleichzeitig geöffnet sein.
    // Weswegen diese Datei geschlossen werden muss bevor man die nächste öffnet
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

      // Schließe die Datei
      myFile.close();
    } 
    else 
    {
      // Die Datei lies sich nicht öffnen - Gebe einen Fehler aus
      Serial.println("error opening test.txt");
    }
    
  }
  
  // Setze den durschnittlichen Distanz-Wert zurück um für die nächte Messung vorbereitet zu sein:
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
