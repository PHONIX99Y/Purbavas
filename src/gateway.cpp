#include <Arduino.h>
#include <ArduinoJson.h>
#include <LoRa.h>
#include <SPI.h>

// ============================================================
// GATEWAY LoRa PIN CONFIGURATION
// ============================================================

#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23

#define LORA_SS 25
#define LORA_RST 26
#define LORA_DIO0 27

// ============================================================
// LoRa FREQUENCY
// Both sensor node and gateway use 433 MHz
// ============================================================

#define LORA_FREQUENCY 433E6

// ============================================================
// PURBAVAS LoRa ALERT PACKET
//
// MUST MATCH THE SENSOR NODE
//
// Total size = 12 bytes
// ============================================================

#pragma pack(push, 1)

typedef struct {

  uint16_t node_id;

  uint8_t hazard_type;
  uint8_t severity_level;
  uint8_t confidence_pct;

  uint8_t flood_risk_pct;
  uint8_t fire_risk_pct;
  uint8_t air_risk_pct;
  uint8_t landslide_risk_pct;
  uint8_t chem_risk_pct;

  uint16_t battery_mv;

} purbavas_lora_alert_packet_t;

#pragma pack(pop)

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("          PURBAVAS GATEWAY              ");
  Serial.println("========================================");

  // ==========================================================
  // SPI CONFIGURATION
  // ==========================================================

  Serial.println();
  Serial.println("Initializing SPI...");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

  Serial.println("SPI:");
  Serial.println("  SCK  = GPIO 18");
  Serial.println("  MISO = GPIO 19");
  Serial.println("  MOSI = GPIO 23");
  Serial.println("  SS   = GPIO 25");

  // ==========================================================
  // LoRa PIN CONFIGURATION
  // ==========================================================

  Serial.println();
  Serial.println("Initializing LoRa...");

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  Serial.println("LoRa pins:");
  Serial.println("  NSS  = GPIO 25");
  Serial.println("  RST  = GPIO 26");
  Serial.println("  DIO0 = GPIO 27");

  // ==========================================================
  // START LoRa
  // ==========================================================

  if (!LoRa.begin(LORA_FREQUENCY)) {

    Serial.println();
    Serial.println("ERROR: LoRa initialization FAILED!");

    while (true) {
      delay(1000);
    }
  }

  Serial.println();
  Serial.println("LoRa frequency : 433 MHz");
  Serial.println("LoRa status    : OK");

  // ==========================================================
  // START RECEIVING
  // ==========================================================

  LoRa.receive();

  Serial.println();
  Serial.println("Gateway is waiting for LoRa packets...");

  Serial.println();
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  // Check whether a LoRa packet has arrived
  int packetSize = LoRa.parsePacket();

  // No packet
  if (packetSize == 0) {
    return;
  }

  Serial.println();
  Serial.println("========================================");
  Serial.println("          LoRa PACKET RECEIVED          ");
  Serial.println("========================================");

  Serial.print("Packet size : ");
  Serial.print(packetSize);
  Serial.println(" bytes");

  // ==========================================================
  // CHECK PACKET SIZE
  // ==========================================================

  if (packetSize != sizeof(purbavas_lora_alert_packet_t)) {

    Serial.println("WARNING: Unexpected packet size!");

    Serial.print("Expected : ");
    Serial.print(sizeof(purbavas_lora_alert_packet_t));
    Serial.println(" bytes");

    Serial.print("Received : ");
    Serial.print(packetSize);
    Serial.println(" bytes");

    // Clear remaining bytes
    while (LoRa.available()) {
      LoRa.read();
    }

    LoRa.receive();

    Serial.println("Ignoring invalid packet.");

    Serial.println("========================================");

    return;
  }

  // ==========================================================
  // READ BINARY PACKET
  // ==========================================================

  purbavas_lora_alert_packet_t packet;

  uint8_t *buffer = reinterpret_cast<uint8_t *>(&packet);

  for (uint8_t i = 0; i < packetSize; i++) {

    buffer[i] = LoRa.read();
  }

  // ==========================================================
  // GET LoRa SIGNAL INFORMATION
  // ==========================================================

  long rssi = LoRa.packetRssi();

  float snr = LoRa.packetSnr();

  // ==========================================================
  // PRINT DECODED DATA
  // ==========================================================

  Serial.println();
  Serial.println("Decoded packet:");
  Serial.println();

  Serial.print("Node ID         : 0x");

  if (packet.node_id < 0x1000) {
    Serial.print("0");
  }

  Serial.println(packet.node_id, HEX);

  Serial.print("Hazard type     : ");
  Serial.println(packet.hazard_type);

  Serial.print("Severity        : ");
  Serial.println(packet.severity_level);

  Serial.print("Confidence      : ");
  Serial.print(packet.confidence_pct);
  Serial.println("%");

  Serial.print("Flood risk      : ");
  Serial.print(packet.flood_risk_pct);
  Serial.println("%");

  Serial.print("Fire risk       : ");
  Serial.print(packet.fire_risk_pct);
  Serial.println("%");

  Serial.print("Air risk        : ");
  Serial.print(packet.air_risk_pct);
  Serial.println("%");

  Serial.print("Landslide risk  : ");
  Serial.print(packet.landslide_risk_pct);
  Serial.println("%");

  Serial.print("Chemical risk   : ");
  Serial.print(packet.chem_risk_pct);
  Serial.println("%");

  Serial.print("Battery         : ");
  Serial.print(packet.battery_mv);
  Serial.println(" mV");

  Serial.print("RSSI            : ");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("SNR             : ");
  Serial.print(snr, 2);
  Serial.println(" dB");

  // ==========================================================
  // CREATE JSON FOR BACKEND
  // ==========================================================

  JsonDocument doc;

  doc["node_id"] = packet.node_id;

  doc["hazard_type"] = packet.hazard_type;

  doc["severity"] = packet.severity_level;

  doc["confidence_pct"] = packet.confidence_pct;

  doc["risk"]["flood"] = packet.flood_risk_pct;

  doc["risk"]["fire"] = packet.fire_risk_pct;

  doc["risk"]["air"] = packet.air_risk_pct;

  doc["risk"]["landslide"] = packet.landslide_risk_pct;

  doc["risk"]["chemical"] = packet.chem_risk_pct;

  doc["battery_mv"] = packet.battery_mv;

  doc["lora"]["rssi"] = rssi;

  doc["lora"]["snr"] = snr;

  String jsonOutput;

  serializeJson(doc, jsonOutput);

  // ==========================================================
  // PRINT JSON
  // ==========================================================

  Serial.println();
  Serial.println("Gateway JSON:");
  Serial.println(jsonOutput);

  Serial.println();
  Serial.println("========================================");
  Serial.println("Packet processing complete.");
  Serial.println("Waiting for next packet...");
  Serial.println("========================================");

  Serial.println();

  // Continue listening
  LoRa.receive();
}