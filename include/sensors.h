#ifndef SENSORS_H
#define SENSORS_H

struct SensorData {
  float temperature;
  float humidity;
  int gas;
  float vibration;
  int water;
  bool irDetected;
  float distance;

  // MPU6050
  float accelerationX;
  float accelerationY;
  float accelerationZ;
  float tiltAngle;
};

SensorData readSensors();

void sensorsInit();

void printSensorData(const SensorData &data);

#endif