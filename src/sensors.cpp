#include "sensors.h"
#include <Adafruit_SHT31.h>
#include <Arduino.h>
#include <Wire.h>

// ---------------- PIN DEFINITIONS ----------------
#define MQ2_PIN 25
#define VIBRATION_PIN 27
#define WATER_PIN 33
#define IR_PIN 12

#define I2C_SDA 21
#define I2C_SCL 22

// HC-SR04
#define TRIG_PIN 32
#define ECHO_PIN 34

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void sensorsInit() {

  // I2C bus
  Wire.begin(I2C_SDA, I2C_SCL);

  // Digital sensor pins
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  // HC-SR04
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // SHT31
  if (!sht31.begin(0x44)) {
    Serial.println("ERROR: SHT31 not found!");
  } else {
    Serial.println("SHT31: OK");
  }
}

SensorData readSensors() {

  SensorData data;

  // -------- SHT31 --------
  data.temperature = sht31.readTemperature();
  data.humidity = sht31.readHumidity();

  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("WARNING: Invalid SHT31 reading!");
    data.temperature = 0.0;
    data.humidity = 0.0;
  }

  // -------- MQ-2 --------
  data.gas = analogRead(MQ2_PIN);

  // -------- SW-420 --------
  data.vibration = digitalRead(VIBRATION_PIN);

  // -------- Water Sensor --------
  data.water = analogRead(WATER_PIN);

  // -------- IR Sensor --------
  data.irDetected = digitalRead(IR_PIN);

  // -------- HC-SR04 --------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration > 0) {
    data.distance = duration * 0.0343 / 2.0;
  } else {
    data.distance = 0.0;
  }

  return data;
}

void printSensorData(const SensorData &data) {

  Serial.println("========== PURBAVAS SENSOR DATA ==========");

  Serial.print("Temperature : ");
  Serial.print(data.temperature, 2);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(data.humidity, 2);
  Serial.println(" %");

  Serial.print("Gas         : ");
  Serial.println(data.gas);

  Serial.print("Vibration   : ");
  Serial.println(data.vibration);

  Serial.print("Water       : ");
  Serial.println(data.water);

  Serial.print("Distance    : ");
  Serial.print(data.distance, 2);
  Serial.println(" cm");

  Serial.print("IR          : ");
  Serial.println(data.irDetected ? "DETECTED" : "CLEAR");

  Serial.println("==========================================");
}