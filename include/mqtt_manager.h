#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include "types.h"
#include <ArduinoJson.h>

// Initialization
bool initMQTT(void);

// Call from loop()
void mqttTask(void);

// Status
bool mqttConnected(void);

// Reconnect
bool mqttReconnect(void);

// Publish
bool mqttPublishEvent(const NodePacket &packet);

bool mqttPublishHeartbeat(void);

bool mqttPublishStatus(void);

bool mqttPublish(const char *topic,
                 const char *payload,
                 bool retained);

bool mqttPublishJSON(const char *topic,
                     JsonDocument &doc,
                     bool retained);

#endif