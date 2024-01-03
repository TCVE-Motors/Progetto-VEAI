/*

  This software is part of Progetto VEAI, TCVE Motors, Ferrara Davide Giacomo.
  Software: veai.ino
  License: GNU General Public License v3

  -----------------------------------------------------------------------------

  Copyright (C) 2023  TCVE Motors.
  
  This file is part of Progetto VEAI

  veai.ino is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  veai.ino is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with veai.ino. If not, see <http://www.gnu.org/licenses/>.

  -----------------------------------------------------------------------------
*/
// LAST UPDATE: 03/01/2024 - Ferrara Davide Giacomo.

#include <DHT.h>       // Includes the DHT library to use the DHT22 sensor.
#include <MQ135.h>     // Includes the MQ135 library to use the MQ-135 sensor.

#define DHTPIN 4       // DHT22 data pin.
#define DHTTYPE DHT11  // DHT sensor type.
DHT dht(DHTPIN, DHTTYPE);

#define MQ135PIN A0    // MQ-135 data pin.
MQ135 mq135 = MQ135(MQ135PIN);

int maxTM = 0;              // Variable to record the maximum temperature of TMP36GT9Z (engine)
int maxG = 0;               // Variable to record the maximum RPM of KY-035 (rpm)
int maxVel = 0;             // Variable to record the maximum speed
int Vel = 0;                // Variable to record the speed
const int buzzerPin = 10;   // Pin variable for the buzzer
const int flamePin = 5;     // Pin variable for KY-026 sensor (flame)
const int hallPin = 2;      // Pin variable for KY-035 sensor (rpm)
const int vibrPin = 3;      // Pin variable for the vibrating motor on the steering wheel in case of an alarm
int digitalVal = 0;         // Variable for the digital value of KY-026 sensor (flame)
float tempM = 0;            // Variable to record real-time temperature of TMP36GT9Z (engine)
float tempE = 0;            // Variable to record real-time temperature of DHT11 (environment)
float tempBM = 0;           // Variable to record real-time temperature of TMP36GT9Z (engine batteries)
float umi = 0;              // Variable to record real-time humidity of DHT11 (environment)
int rpm = 0;                // Variable to record real-time RPM of KY-035 (rpm)
float Co2 = 0;              // Variable to record real-time CO2 concentration of MQ135 (environment)
unsigned long tempoIniziale = 0;   // Variable to initialize the initial time from KY-035 sensor pulses (rpm)
const int impulsiGiro = 1;         // Variable to declare how many pulses correspond to 1 wheel rotation
const float pi = 3.14159;          // Variable to specify the value of pi
const float diametroRuotaCm = 39;  // Variable to declare the wheel diameter in centimeters
const float circonferenzaRuotaCm = pi * diametroRuotaCm;      // Variable for the wheel circumference in centimeters
const float circonferenzaRuotaM = circonferenzaRuotaCm / 100; // Variable for the wheel circumference in meters
int statoFiamma = 0; // Variable to identify the safety state (Fire)
int statoFumo = 0;   // Variable to identify the safety state (Smoke)
int statoTemp = 0;   // Variable to identify the safety state (Overheating)

void setup() {
  Serial.begin(9600);          // Serial communication initialization, set at a rate of 9600 baud
  dht.begin();                 // DHT22 sensor initialization
  pinMode(flamePin, INPUT);    // Set pin as INPUT
  pinMode(MQ135PIN, INPUT);    // Set pin as INPUT
  pinMode(hallPin, INPUT);     // Set pin as INPUT
  pinMode(buzzerPin, OUTPUT);  // Set pin as OUTPUT
  tempoIniziale = millis();    // Declaration of variable value
}

void loop() {

  if (!Serial.available()) {        // Condition: Serial communication is unsuccessful
    digitalWrite(vibrPin, HIGH);       // Activation of vibration on the steering wheel via dedicated motors.
    tone(buzzerPin, 2000, 500);        // Effect: The speaker will be activated
    delay(500);
    tone(buzzerPin, 1500, 500);    
    delay(500);
    tone(buzzerPin, 1000, 500);
    delay(500);
    tone(buzzerPin, 500, 500);
    delay(500);
  }
  digitalVal = digitalRead(flamePin);    // Declaration of variable value
  if (digitalVal == HIGH){               // Condition: Flame detected in the engine compartment
    digitalWrite(vibrPin, HIGH);         // Activation of vibration on the steering wheel via dedicated motors.
    tone(buzzerPin, 1500, 500);          // Effect: The speaker will be activated
    delay(500);
    tone(buzzerPin, 1000, 500);
    delay(500);
    tone(buzzerPin, 500, 500);
    delay(500);
    statoFiamma = 1;
  }

  if (Co2 > 800) {                     // Condition: CO2 concentration exceeds 800 ppm
    digitalWrite(vibrPin, HIGH);       // Activation of vibration on the steering wheel via dedicated motors.
    tone(buzzerPin, 1500, 500);        // Effect: The speaker will be activated
    delay(500);
    tone(buzzerPin, 1000, 500);
    delay(500);
    statoFumo = 1;
  }

  if (tempM > 80) {                    // Condition: Engine temperature exceeds 80°C
    digitalWrite(vibrPin, HIGH);       // Activation of vibration on the steering wheel via dedicated motors.
    tone(buzzerPin, 1500, 500);        // Effect: The speaker will be activated
    delay(500);
    statoTemp = 1;
  }

  if (tempBM > 60) {                   // Condition: Engine battery temperature exceeds 60°C
    digitalWrite(vibrPin, HIGH);       // Activation of vibration on the steering wheel via dedicated motors.
    tone(buzzerPin, 1500, 500);        // Effect: The speaker will be activated
    delay(500);
    statoTemp = 1;
  }

  // Acquiring data from DHT11 sensor
  tempE = dht.readTemperature();
  umi = dht.readHumidity();

  // Acquiring data from MQ-135 sensor
  Co2 = mq135.getPPM();

  // Acquiring data from TMP36GT9Z sensor
  tempM = (analogRead(A2) * 0.48828125 - 50.0);
  tempBM = (analogRead(A4) * 0.01 - 50.0);

  // Check if the KY-035 hall sensor has sent a signal (complete wheel rotation)
  if (digitalRead(hallPin) == HIGH) {
    // Calculate the time elapsed since the program started
    unsigned long tempoTrascorso = millis() - tempoIniziale;

    // Use the elapsed time to calculate wheel speed or perform other operations
    rpm = (60.0 * 1000.0) / (tempoTrascorso * impulsiGiro);
    // Calculate the distance covered in a minute (in meters)
    float distanzaPercorsaMetri = circonferenzaRuotaM * rpm * 60;
    // Convert the distance covered in an hour (in kilometers)
    Vel = distanzaPercorsaMetri / 1000;

    // Reset the initial time for the next calculation
    tempoIniziale = millis();
  }

  // Check and record maximum values
  if (tempM > maxTM) {
    maxTM = tempM;
  }
  if (rpm > maxG) {
    maxG = rpm;
  }
  if (Vel > maxVel) {
    maxVel = Vel;
  }

  // Print real-time values to serial for display communication
  Serial.print(" Temperatura motore: ");
  Serial.print(tempM);
  Serial.print(" °C");

  Serial.print(" Temperatura Motore Massima: ");
  Serial.print(maxTM);
  Serial.print(" °C");

  Serial.print(" Temperatura Batterie Motore: ");
  Serial.print(tempBM);
  Serial.print(" °C");

  Serial.print(" Temperatura Esterna: ");
  Serial.print(tempE);
  Serial.print(" °C");

  Serial.print(" Umidità: ");
  Serial.print(umi);
  Serial.print(" %");

  Serial.print(" Concentrazione di CO2: ");
  Serial.print(Co2);
  Serial.print(" PPM");

  Serial.print(" RPM: ");
  Serial.print(rpm);

  Serial.print(" RPM Massimi: ");
  Serial.print(maxG);

  Serial.print(" Velocità: ");
  Serial.print(Vel);
  Serial.print(" Km/h");

  Serial.print(" Velocità Massima: ");
  Serial.print(maxVel);
  Serial.print(" Km/h");

  Serial.print(" Stato allarmi: ");
  Serial.print(statoFiamma);
  Serial.print(statoFumo);
  Serial.print(statoTemp);

  Serial.println();
  delay(500);
}
