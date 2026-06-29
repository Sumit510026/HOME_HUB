#include "wifi_storage.h"

#include <LittleFS.h>
#include <ArduinoJson.h>

//======================================================
// Configuration
//======================================================

#define WIFI_FILE "/wifi.json"

//======================================================
// Initialize LittleFS
//======================================================

bool initWiFiStorage(void)
{
    if (!LittleFS.begin())
    {
        Serial.println("[Storage] LittleFS Mount Failed");
        return false;
    }

    Serial.println("[Storage] LittleFS Mounted");

    return true;
}

//======================================================
// Save WiFi Credentials
//======================================================

bool saveWiFiCredentials(const String &ssid,
                         const String &password)
{
    File file = LittleFS.open(WIFI_FILE, "w");

    if (!file)
    {
        Serial.println("[Storage] Failed to create wifi.json");
        return false;
    }

    JsonDocument doc;

    doc["ssid"] = ssid;
    doc["password"] = password;

    if (serializeJson(doc, file) == 0)
    {
        Serial.println("[Storage] Failed to write JSON");

        file.close();

        return false;
    }

    file.flush();
    file.close();

    Serial.println("[Storage] Credentials Saved");

    Serial.print("[Storage] SSID : ");
    Serial.println(ssid);

    return true;
}

//======================================================
// Load WiFi Credentials
//======================================================

bool loadWiFiCredentials(WiFiCredentials &credentials)
{
    if (!LittleFS.exists(WIFI_FILE))
    {
        Serial.println("[Storage] wifi.json not found");
        return false;
    }

    File file = LittleFS.open(WIFI_FILE, "r");

    if (!file)
    {
        Serial.println("[Storage] Failed to open wifi.json");
        return false;
    }

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);

    file.close();

    if (error)
    {
        Serial.print("[Storage] JSON Parse Failed : ");
        Serial.println(error.c_str());

        return false;
    }

    credentials.ssid     = doc["ssid"] | "";
    credentials.password = doc["password"] | "";

    if (credentials.ssid.isEmpty())
    {
        Serial.println("[Storage] Invalid Credentials");

        return false;
    }

    Serial.println("[Storage] Credentials Loaded");

    Serial.print("[Storage] SSID : ");
    Serial.println(credentials.ssid);

    return true;
}

//======================================================
// Credentials Exist
//======================================================

bool wifiCredentialsExist(void)
{
    return LittleFS.exists(WIFI_FILE);
}

//======================================================
// Erase Credentials
//======================================================

bool eraseWiFiCredentials(void)
{
    if (!LittleFS.exists(WIFI_FILE))
    {
        Serial.println("[Storage] No Credentials Found");
        return true;
    }

    if (LittleFS.remove(WIFI_FILE))
    {
        Serial.println("[Storage] Credentials Deleted");
        return true;
    }

    Serial.println("[Storage] Failed to Delete Credentials");

    return false;
}