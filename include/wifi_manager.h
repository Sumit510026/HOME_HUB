#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

bool initWiFi(void);

void wifiTask(void);

bool wifiConnected(void);

int32_t wifiRSSI(void);

String wifiMAC(void);

String wifiIP(void);

#endif