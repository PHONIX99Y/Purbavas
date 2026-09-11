#include "purbavas_edge_model.h"
#include "purbavas_lora.h"
#include "sensor_packet.h"
#include "sensors.h"

#include <Arduino.h>
#include <math.h>

const unsigned long SENSOR_INTERVAL = 2000;
unsigned long lastSensorRead = 0;

void setup() {

  Serial.begin(115200);
  delay(1000);

  loraInit();
  sensorsInit();

  Serial.println();
  Serial.println("================================");
  Serial.println("      PURBAVAS SENSOR NODE      ");
  Serial.println("================================");
  Serial.println("Hardware sensor + edge model mode");
  Serial.println();
}

void loop() {

  unsigned long currentTime = millis();

  if (currentTime - lastSensorRead >= SENSOR_INTERVAL) {

    lastSensorRead = currentTime;

    // --------------------------------
    // 1. Read physical sensors
    // --------------------------------
    SensorData data = readSensors();

    printSensorData(data);

    // --------------------------------
    // 2. Create edge-model input
    // --------------------------------
    purbavas_raw_telemetry_t raw = {};

    // SHT31
    raw.temperature_c = data.temperature;
    raw.humidity_pct = data.humidity;

    // HC-SR04
    // Temporary mapping:
    // distance in cm -> water level in metres
    raw.water_level_m = data.distance / 100.0f;

    // Water-level change is not measured yet
    raw.water_level_rate_m_hr = 0.0f;

    // MQ-2
    // Currently raw ADC value.
    // This is NOT calibrated ppm yet.
    raw.smoke_ppm = (float)data.gas;

    // MPU6050
    raw.vibration_g = fabs(data.vibration - 1.0f);
    raw.tilt_angle_deg = data.tiltAngle;

    // --------------------------------
    // 3. Inputs for sensors we don't
    // currently have
    // --------------------------------

    raw.elevation_m = 0.0f;

    raw.rainfall_1h_mm = 0.0f;
    raw.rainfall_6h_mm = 0.0f;
    raw.rainfall_24h_mm = 0.0f;

    raw.heat_index_c = 0.0f;

    raw.co_ppm = 0.0f;

    raw.voc_ppb = 0.0f;

    raw.so2_ug_m3 = 0.0f;
    raw.no2_ug_m3 = 0.0f;

    raw.pm25 = 0.0f;
    raw.pm10 = 0.0f;
    raw.aqi_calculated = 0.0f;

    raw.soil_moisture_pct = 0.0f;

    // Neutral water-quality values
    raw.water_ph = 7.0f;
    raw.water_turbidity_ntu = 0.0f;
    raw.dissolved_oxygen_mg_l = 7.0f;

    raw.battery_voltage_v = 0.0f;
    raw.signal_rssi_dbm = 0.0f;

    // --------------------------------
    // 4. Run edge model
    // --------------------------------
    purbavas_inference_result_t result = purbavas_predict_edge(&raw);

    // --------------------------------
    // 5. Display edge-model result
    // --------------------------------
    Serial.println("========== EDGE MODEL ==========");

    Serial.print("Hazard     : ");
    Serial.println(purbavas_hazard_to_string(result.predicted_hazard));

    Serial.print("Severity   : ");
    Serial.println(purbavas_severity_to_string(result.severity));

    Serial.print("Confidence : ");
    Serial.print(result.confidence);
    Serial.println("%");

    Serial.print("Broadcast  : ");
    Serial.println(result.should_broadcast_alert ? "YES" : "NO");

    Serial.print("Anomaly    : ");
    Serial.println(result.is_anomaly ? "YES" : "NO");

    Serial.println("================================");

    // --------------------------------
    // 6. Pack edge result for LoRa
    // --------------------------------
    uint8_t loraPacket[sizeof(purbavas_lora_alert_packet_t)];

    uint16_t batteryMv = 0;

    uint8_t packetLength = purbavas_pack_lora_alert(
        &result, 0x0101, batteryMv, loraPacket, sizeof(loraPacket));

    // --------------------------------
    // 7. Send binary LoRa packet
    // --------------------------------
    if (packetLength > 0) {

      if (loraSendBytes(loraPacket, packetLength)) {
        Serial.println("LoRa alert packet sent successfully.");
      } else {
        Serial.println("LoRa alert packet transmission failed.");
      }

    } else {

      Serial.println("ERROR: Failed to create LoRa alert packet.");
    }

    Serial.println();
  }
}