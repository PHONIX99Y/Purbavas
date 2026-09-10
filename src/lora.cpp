#include "purbavas_lora.h"
#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26

void loraInit() {

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  Serial.println("Initializing SX1278 LoRa...");

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa initialization FAILED!");
    return;
  }

  Serial.println("LoRa initialization SUCCESS!");
}

bool loraSend(const String &packet) {

  LoRa.beginPacket();
  LoRa.print(packet);

  int result = LoRa.endPacket();

  return result == 1;
}