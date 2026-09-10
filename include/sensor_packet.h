#ifndef SENSOR_PACKET_H
#define SENSOR_PACKET_H

#include "sensors/sensors.h"
#include <Arduino.h>

// SensorPacket represents the telemetry packet for network/LoRa transmission
typedef SensorData SensorPacket;

// Serialize sensor packet into JSON string
String createPacket(const SensorPacket &data);

#endif // SENSOR_PACKET_H