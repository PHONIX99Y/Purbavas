#include "sensors.h"
#include <Arduino.h>

SensorData readSimulatedSensors() {
  SensorData data;

  data.temperature = 25.0 + random(0, 100) / 100.0;
  data.humidity = 60.0 + random(0, 200) / 100.0;
  data.gas = random(200, 700);
  data.vibration = random(0, 100) / 100.0;
  data.distance = random(50, 500) / 100.0;
  data.irDetected = random(0, 10) > 7;

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
  Serial.println(data.vibration, 2);

  Serial.print("Distance    : ");
  Serial.print(data.distance, 2);
  Serial.println(" cm");

  Serial.print("IR          : ");
  Serial.println(data.irDetected ? "DETECTED" : "CLEAR");

  Serial.println("==========================================");
}