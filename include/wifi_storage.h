#ifndef WIFI_STORAGE_H
#define WIFI_STORAGE_H

#include <Arduino.h>

//======================================================
// WiFi Credentials Structure
//======================================================

struct WiFiCredentials
{
    String ssid;
    String password;
};

//======================================================
// Initialize Storage
//======================================================

// Initializes LittleFS.
// Must be called once during startup.
bool initWiFiStorage(void);

//======================================================
// Save Credentials
//======================================================

// Saves SSID and Password to LittleFS.
// Returns true on success.
bool saveWiFiCredentials(const String &ssid,
                         const String &password);

//======================================================
// Load Credentials
//======================================================

// Loads SSID and Password from LittleFS.
// Returns true if credentials exist.
bool loadWiFiCredentials(WiFiCredentials &credentials);

//======================================================
// Check Credentials
//======================================================

// Returns true if valid credentials are stored.
bool wifiCredentialsExist(void);

//======================================================
// Delete Credentials
//======================================================

// Deletes the saved WiFi credentials.
bool eraseWiFiCredentials(void);

#endif // WIFI_STORAGE_H