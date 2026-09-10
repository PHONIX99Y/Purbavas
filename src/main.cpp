#include "lora/lora.h"
#include "protocol/sensor_packet.h"
#include "sensors/sensors.h"
#include <Arduino.h>

const unsigned long SENSOR_INTERVAL = 2000;
unsigned long lastSensorRead = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  randomSeed(analogRead(A0));

  Serial.println();
  Serial.println("================================");
  Serial.println("      PURBAVAS SENSOR NODE      ");
  Serial.println("================================");
  Serial.println("Simulation mode");
  Serial.println();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = currentTime;

    // Read sensors
    SensorData data = readSimulatedSensors();

    // Display readings
    printSensorData(data);

    // Create packet
    String packet = createPacket(data);

    if (loraSend(packet)) {
      Serial.println("LoRa packet sent successfully.");
    } else {
      Serial.println("LoRa packet transmission failed.");
    }
    Serial.println();
  }
}