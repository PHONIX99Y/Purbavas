#ifndef PURBAVAS_LORA_H
#define PURBAVAS_LORA_H

#include <Arduino.h>

void loraInit();
bool loraSend(const String &packet);

#endif