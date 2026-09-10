#include "sensor_packet.h"
#include <ArduinoJson.h>

String createPacket(const SensorPacket &data) {

  JsonDocument doc;

  doc["temperature"] = serialized(String(data.temperature, 2));
  doc["humidity"] = serialized(String(data.humidity, 2));
  doc["gas"] = data.gas;
  doc["water"] = data.water;
  doc["vibration"] = serialized(String(data.vibration, 2));
  doc["distance"] = serialized(String(data.distance, 2));
  doc["ir_detected"] = data.irDetected;

  String packet;

  serializeJson(doc, packet);

  return packet;
}