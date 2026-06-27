#include "wifi_manager.h"

#include <ESP8266WiFi.h>

#include "config.h"

//======================================================
// Configuration
//======================================================

static uint32_t lastReconnectAttempt = 0;

static const uint32_t RECONNECT_INTERVAL = 10000;     // 10 sec
static const uint32_t WIFI_TIMEOUT = 15000;           // 15 sec

//======================================================
// Connect WiFi
//======================================================

static bool connectWiFi(void)
{
    Serial.println();
    Serial.println("[WiFi] Connecting...");

    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t start = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");

        if ((millis() - start) > WIFI_TIMEOUT)
        {
            Serial.println();
            Serial.println("[WiFi] Connection Timeout");

            return false;
        }
    }

    Serial.println();
    Serial.println("[WiFi] Connected");

    Serial.print("[WiFi] IP : ");
    Serial.println(WiFi.localIP());

    Serial.print("[WiFi] MAC : ");
    Serial.println(WiFi.macAddress());

    Serial.print("[WiFi] RSSI : ");
    Serial.println(WiFi.RSSI());

    return true;
}

//======================================================
// Initialize
//======================================================

bool initWiFi(void)
{
    return connectWiFi();
}

//======================================================
// Task
//======================================================

void wifiTask(void)
{
    static wl_status_t last = WL_CONNECTED;

    wl_status_t wifiStatus = WiFi.status();

    if (wifiStatus != last)
    {
        Serial.printf("[WiFi] Status=%d\n", wifiStatus);
        last = wifiStatus;
    }

    if (wifiStatus == WL_CONNECTED)
        return;

    uint32_t currentTime = millis();

    if ((currentTime - lastReconnectAttempt) < RECONNECT_INTERVAL)
        return;

    lastReconnectAttempt = currentTime;

    Serial.println("[WiFi] Disconnected");

    connectWiFi();
}

//======================================================
// Status
//======================================================

bool wifiConnected(void)
{
    return (WiFi.status() == WL_CONNECTED);
}

//======================================================
// RSSI
//======================================================

int32_t wifiRSSI(void)
{
    return WiFi.RSSI();
}

//======================================================
// MAC
//======================================================

String wifiMAC(void)
{
    return WiFi.macAddress();
}

//======================================================
// IP
//======================================================

String wifiIP(void)
{
    return WiFi.localIP().toString();
}