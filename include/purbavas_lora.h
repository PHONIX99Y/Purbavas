#ifndef PURBAVAS_LORA_H
#define PURBAVAS_LORA_H

#include <Arduino.h>

void loraInit();
bool loraSend(const String &packet);
bool loraSendBytes(const uint8_t *data, uint8_t length);

#endif