#ifndef LORA_H
#define LORA_H

#include <Arduino.h>

void loraInit();
bool loraSend(const String &packet);

#endif