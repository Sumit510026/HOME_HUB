#include "wifi_config.h"
#include "wifi_storage.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//======================================================
// Configuration
//======================================================

#define CONFIG_AP_SSID     "SecureHome_Setup"
#define CONFIG_AP_PASSWORD ""

ESP8266WebServer server(80);

static bool portalRunning = false;

//======================================================
// HTML Page
//======================================================

static const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Secure Home Setup</title>
<style>
body{
    font-family:Arial;
    text-align:center;
    margin-top:40px;
}
input{
    width:250px;
    padding:10px;
    margin:8px;
}
button{
    padding:10px 20px;
}
</style>
</head>

<body>

<h2>Secure Home WiFi Setup</h2>

<form action="/save" method="POST">

<input
type="text"
name="ssid"
placeholder="WiFi SSID"
required>

<br>

<input
type="password"
name="password"
placeholder="WiFi Password">

<br><br>

<button type="submit">
Save
</button>

</form>

</body>
</html>
)rawliteral";

//======================================================
// Root Page
//======================================================

static void handleRoot()
{
    server.send(200, "text/html", HTML_PAGE);
}

//======================================================
// Save Credentials
//======================================================

static void handleSave()
{
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    Serial.println();
    Serial.println("========== WiFi Configuration ==========");

    Serial.print("SSID : ");
    Serial.println(ssid);

    Serial.print("Password : ");
    Serial.println(password);

    if (saveWiFiCredentials(ssid, password))
    {
        server.send(200,
                    "text/html",
                    "<h2>Credentials Saved.</h2>"
                    "<h3>Device will restart...</h3>");

        delay(2000);
        stopWiFiConfigPortal();

        delay(500);

        ESP.restart();
    }
    else
    {
        server.send(500,
                    "text/html",
                    "<h2>Failed to save credentials.</h2>");
    }
}

//======================================================
// Start Portal
//======================================================

bool startWiFiConfigPortal(void)
{
    Serial.println();
    Serial.println("[WiFi Config] Starting Portal");

    WiFi.mode(WIFI_AP);

    WiFi.softAP(CONFIG_AP_SSID,
                CONFIG_AP_PASSWORD);

    Serial.print("[WiFi Config] AP IP : ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);

    server.on("/save", HTTP_POST, handleSave);

    server.begin();

    portalRunning = true; //before change its true, so that wifiConfigTask() can be called in loop()

    Serial.println("[WiFi Config] Web Server Started");

    return true;
}

//======================================================
// Handle Clients
//======================================================

void wifiConfigTask(void)
{
    if (!portalRunning)
        return;

    server.handleClient();

    yield();
}

//======================================================
// Portal Running?
//======================================================

bool wifiConfigPortalRunning(void)
{
    return portalRunning;
}

//======================================================
// Stop Portal
//======================================================

void stopWiFiConfigPortal(void)
{
    server.stop();

    WiFi.softAPdisconnect(true);

    portalRunning = false;

    Serial.println("[WiFi Config] Portal Stopped");
}