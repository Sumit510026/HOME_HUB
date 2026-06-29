#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <Arduino.h>

//======================================================
// Configuration Portal
//======================================================

// Starts the Wi-Fi configuration portal.
// Returns true if Wi-Fi credentials were saved successfully.
// Returns false if the user closed the portal or it timed out.
bool startWiFiConfigPortal(void);

//======================================================
// Web Server Task
//======================================================

// Handles incoming HTTP requests.
// Call this continuously while the configuration portal is active.
void wifiConfigTask(void);

//======================================================
// Status
//======================================================

// Returns true if the configuration portal is currently running.
bool wifiConfigPortalRunning(void);

//======================================================
// Stop Portal
//======================================================

// Stops the configuration portal and web server.
void stopWiFiConfigPortal(void);

#endif