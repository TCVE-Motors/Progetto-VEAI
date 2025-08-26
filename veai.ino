/*

  This software is part of Progetto VEAI, TCVE Motors, Ferrara Davide Giacomo.
  Software: veai.ino
  License: GNU General Public License v3

  -----------------------------------------------------------------------------

  Copyright (C) 2024  Ferrara Davide Giacomo.
  
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
// LAST UPDATE: 26/08/2025 - Ferrara Davide Giacomo. V0.1.1

#include <DHT.h>       // Includes the DHT library to use the DHT22 sensor.
#include <MQ135.h>     // Includes the MQ135 library to use the MQ-135 sensor.

#define MOSFET_CABIN     6 // Cabin lighting MOSFET's output pin
#define MOSFET_EXTERNAL  7 // External lighting MOSFET's output pin
#define MOSFET_FANS      8 // Cabin fans MOSFET's output pin
#define MOSFET_HEATING   9 // Cabin heating MOSFET's output pin
#define DHTPIN 4       // DHT11 pin.
#define DHTTYPE DHT11  // DHT sensor type.
DHT dht(DHTPIN, DHTTYPE);
#define MQ135PIN A0    // MQ-135 sensor pin.
MQ135 mq135 = MQ135(MQ135PIN);

struct Sensor {
  uint8_t id;
  const char* name;
  float value;
};
#define NUM_SENSORS 10


Sensor sensors[NUM_SENSORS] = {
    {0, "EngineRR Temp", 0},
    {1, "EngineFR Temp", 0},
    {2, "EngineRL Temp", 0},
    {3, "EngineFL Temp", 0},
    {4, "Battery Temp", 0},
    {5, "Env Temp", 0},
    {6, "Env Humi", 0},
    {7, "CO2", 0},
    {8, "RPM", 0},
    {9, "Flame", 0},
};

const int NUM_MOSFETS = 4;
int mosfetPins[NUM_MOSFETS] = {
    MOSFET_CABIN,
    MOSFET_EXTERNAL,
    MOSFET_FANS,
    MOSFET_HEATING
};
int mosfetStates[NUM_MOSFETS] = {0,0,0,0}; // store current state

const int flamePin = 5;
const int hallPin = 2;

volatile unsigned long lastPulseTime = 0;
volatile unsigned int rpm = 0;
const int impulsesPerRevolution = 1;

void hallISR() {
    unsigned long now = micros();  // higher precision
    unsigned long delta = now - lastPulseTime; // microseconds
    if (delta > 0) {
        rpm = 60.0 * 1e6 / (delta * impulsesPerRevolution); 
    }
    lastPulseTime = now;
}


void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(flamePin, INPUT);
    pinMode(hallPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(hallPin), hallISR, RISING);
    // Set MOSFET pins as outputs
    for (int i = 0; i < NUM_MOSFETS; i++) {
        pinMode(mosfetPins[i], OUTPUT);
        digitalWrite(mosfetPins[i], LOW); // all OFF at startup
    }
}

void handleCommands() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');  // read one command line
        cmd.trim();

        int commaIndex = cmd.indexOf(',');
        if (commaIndex > 0) {
            int device = cmd.substring(0, commaIndex).toInt();
            int state  = cmd.substring(commaIndex + 1).toInt();

            if (device >= 0 && device < NUM_MOSFETS) {
                mosfetStates[device] = state ? 1 : 0;
                digitalWrite(mosfetPins[device], mosfetStates[device] ? HIGH : LOW);
            }
        }
    }
}

void readSensors() {
    // Engine temps (TMP36GT9Z sensors)
    sensors[0].value = analogRead(A0); // EngineRR Temp
    sensors[1].value = analogRead(A1); // EngineFR Temp
    sensors[2].value = analogRead(A2); // EngineRL Temp
    sensors[3].value = analogRead(A3); // EngineFL Temp

    // Battery temp
    sensors[4].value = analogRead(A4);

    // DHT11 sensors
    sensors[5].value = dht.readTemperature();
    sensors[6].value = dht.readHumidity();

    // CO2
    // sensors[7].value = mq135.getPPM(); ERROR: DEDICATED ANALOG PIN NEEDED, CONFLICT WITH SENSOR 0

    // RPM / Speed – raw hall sensor state
    sensors[8].value = rpm; // Pi will calculate RPM

    // Flame
    sensors[9].value = analogRead(A5);
}

void sendSensorFrame(Sensor s) {
    uint8_t frame[12];
    frame[0] = 0xAA;  // STX
    frame[1] = s.id;

    uint16_t val = (uint16_t)(s.value * 10);  // cast to 16-bit integer, increase size by 10 to avoid decimal truncation
    frame[2] = val >> 8;
    frame[3] = val & 0xFF;

    // Timestamp (Pi will update later, but could include RTC if available)
    int year = 2025, month = 8, day = 26, hour = 14, minute = 30, second = 0;
    frame[4] = year - 2000;
    frame[5] = month;
    frame[6] = day;
    frame[7] = hour;
    frame[8] = minute;
    frame[9] = second;

    // Checksum
    uint8_t chk = 0;
    for (int i = 1; i <= 9; i++) chk ^= frame[i];
    frame[10] = chk;

    frame[11] = 0x55; // ETX

    Serial.write(frame, 12);
}

void sendAllSensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        sendSensorFrame(sensors[i]);
        delay(5); // small delay to avoid UART overflow
    }
}

void loop() {
    handleCommands();   // check if Pi sent a command
    readSensors();     // just read the raw sensor values
    sendAllSensors();  // send all sensor data to Raspberry Pi
    delay(100);        // adjust frequency (e.g., 10 Hz)
}
