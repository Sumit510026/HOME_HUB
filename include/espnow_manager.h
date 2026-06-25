#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>

bool initESPNow(void);

void espnowTask(void);

// Statistics
uint32_t espnowPacketsReceived(void);
uint32_t espnowPacketsDropped(void);
uint32_t espnowInvalidPackets(void);

#endif