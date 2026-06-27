/*
 * ============================================================
 * HOME SECURITY HUB
 * Main Application
 * ============================================================
 */

#include <Arduino.h>

#include "wifi_manager.h"
#include "espnow_manager.h"
#include "mqtt_manager.h"
#include "heartbeat.h"
#include "ota_manager.h"
#include "aws_manager.h" 
#include "queue_manager.h"

void setup()
{
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("      HOME SECURITY HUB STARTING");
    Serial.println("==========================================");

    initWiFi();
    queueInit();
    initESPNow();
    

    if (!initAWS())
    {
    Serial.println("[SYSTEM] AWS Initialization Failed");

    while (true)
    {
        delay(1000);
    }
    }

    initMQTT();

    initHeartbeat();

    initOTA();

    Serial.println("[SYSTEM] Initialization Complete");
}

void loop()
{
    wifiTask();

    mqttTask();

    espnowTask();

    heartbeatTask();

    otaTask();
}