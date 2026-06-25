/*
 * ============================================================
 * Heartbeat Manager
 * ============================================================
 *
 * Publishes Hub status periodically.
 *
 * ============================================================
 */

#include "heartbeat.h"

#include "mqtt_manager.h"
#include "wifi_manager.h"

#include <ESP8266WiFi.h>

static uint32_t lastHeartbeat = 0;

static const uint32_t HEARTBEAT_INTERVAL = 60000;

//-------------------------------------------------------------

void initHeartbeat(void)
{
    lastHeartbeat = millis();

    Serial.println("[Heartbeat] Initialized");
}

//-------------------------------------------------------------

void heartbeatTask(void)
{
    if (!wifiConnected())
        return;

    if (!mqttConnected())
        return;

    uint32_t now = millis();

    if ((now - lastHeartbeat) >= HEARTBEAT_INTERVAL)
    {
        lastHeartbeat = now;

        if (mqttPublishHeartbeat())
        {
            Serial.println("[Heartbeat] Published");
        }
        else
        {
            Serial.println("[Heartbeat] Publish Failed");
        }
    }
}