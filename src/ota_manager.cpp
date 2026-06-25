/*
 * ============================================================
 * OTA Manager
 * Home Security Hub
 * ============================================================
 */

#include "ota_manager.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "wifi_manager.h"

//------------------------------------------------------------
// Private Variables
//------------------------------------------------------------

static OTAStatus otaStatus = OTA_IDLE;

static bool otaRunning = false;

static uint8_t otaProgress = 0;

//------------------------------------------------------------
// OTA Callbacks
//------------------------------------------------------------

static void otaStartCallback();
static void otaEndCallback();
static void otaProgressCallback(int current, int total);
static void otaErrorCallback(int error);

//------------------------------------------------------------
// Initialize
//------------------------------------------------------------

bool initOTA(void)
{
    otaStatus = OTA_IDLE;

    otaRunning = false;

    otaProgress = 0;

    Serial.println("[OTA] Manager Initialized");
    printFirmwareInfo();
    return true;
}

//------------------------------------------------------------
// OTA Task
//------------------------------------------------------------

void otaTask(void)
{
    /*
     * Reserved
     *
     * Future:
     *  - OTA timeout
     *  - Progress publish
     *  - Retry logic
     */
}

//------------------------------------------------------------
// Firmware Version
//------------------------------------------------------------

const char *getFirmwareVersion(void)
{
    return FW_VERSION;
}

//------------------------------------------------------------
// Compare Version
//------------------------------------------------------------

bool isNewVersionAvailable(const char *newVersion)
{
    if (newVersion == nullptr)
        return false;

    if (strcmp(newVersion, FW_VERSION) == 0)
    {
        Serial.println("[OTA] Latest Firmware");

        return false;
    }

    return true;
}

//------------------------------------------------------------
// Start OTA
//------------------------------------------------------------

bool startOTA(const char *firmwareURL,
              const char *newVersion)
{
    if (otaRunning)
    {
        Serial.println("[OTA] Already Running");

        return false;
    }

    if (!wifiConnected())
    {
        Serial.println("[OTA] WiFi Not Connected");

        return false;
    }

    if (!isNewVersionAvailable(newVersion))
    {
        return false;
    }

    Serial.println();
    Serial.println("=======================================");
    Serial.println("Starting OTA Update");
    Serial.println("=======================================");

    Serial.print("Current Version : ");
    Serial.println(FW_VERSION);

    Serial.print("New Version     : ");
    Serial.println(newVersion);

    Serial.print("Firmware URL    : ");
    Serial.println(firmwareURL);

    otaRunning = true;

    otaStatus = OTA_DOWNLOADING;

    otaProgress = 0;

    //--------------------------------------------------------
    // HTTPS Client
    //--------------------------------------------------------

    BearSSL::WiFiClientSecure client;

    /*
     * TODO:
     * Replace setInsecure() with your
     * OTA Server Root Certificate.
     */

    client.setInsecure();

    //--------------------------------------------------------

    ESPhttpUpdate.onStart(otaStartCallback);

    ESPhttpUpdate.onEnd(otaEndCallback);

    ESPhttpUpdate.onProgress(otaProgressCallback);

    ESPhttpUpdate.onError(otaErrorCallback);

    ESPhttpUpdate.setFollowRedirects(
        HTTPC_FORCE_FOLLOW_REDIRECTS);

    ESPhttpUpdate.rebootOnUpdate(true);

    //--------------------------------------------------------

    t_httpUpdate_return result =
        ESPhttpUpdate.update(
            client,
            firmwareURL);

    switch (result)
    {
        case HTTP_UPDATE_FAILED:

            otaStatus = OTA_FAILED;

            otaRunning = false;

            return false;

        case HTTP_UPDATE_NO_UPDATES:

            otaStatus = OTA_IDLE;

            otaRunning = false;

            return false;

        case HTTP_UPDATE_OK:

            /*
             * Never reaches here.
             *
             * ESP reboots automatically.
             */

            otaStatus = OTA_SUCCESS;

            return true;
    }

    otaRunning = false;

    return false;
}

//------------------------------------------------------------
// OTA Callbacks
//------------------------------------------------------------

static void otaStartCallback()
{
    otaStatus = OTA_DOWNLOADING;

    otaProgress = 0;

    Serial.println();
    Serial.println("================================");
    Serial.println("[OTA] Download Started");
    Serial.println("================================");

    Serial.print("[OTA] Free Heap : ");
    Serial.println(ESP.getFreeHeap());

    Serial.print("[OTA] Sketch Size : ");
    Serial.println(ESP.getSketchSize());

    Serial.print("[OTA] Free Space : ");
    Serial.println(ESP.getFreeSketchSpace());
}

static void otaEndCallback()
{
    otaStatus = OTA_SUCCESS;

    otaProgress = 100;

    otaRunning = false;

    Serial.println();

    Serial.println("================================");

    Serial.println("[OTA] Update Successful");

    Serial.println("[OTA] Rebooting...");

    Serial.println("================================");
}

static void otaProgressCallback(int current,
                                int total)
{
    if (total == 0)
        return;

    uint8_t percent =
        (current * 100) / total;

    otaProgress = percent;

    if (percent != lastPercent)
    {
        lastPercent = percent;

        Serial.printf(
            "[OTA] %u%%\n",
            percent);
    }
}

static void otaErrorCallback(int error)
{
    otaStatus = OTA_FAILED;

    otaRunning = false;

    Serial.println();

    Serial.println("================================");

    Serial.println("[OTA] Update Failed");

    Serial.printf("Error Code : %d\n", error);

    Serial.print("Reason : ");

    Serial.println(
        ESPhttpUpdate.getLastErrorString());

    Serial.println("================================");
}

void printFirmwareInfo()
{
    Serial.println();

    Serial.println("========== Firmware ==========");

    Serial.print("Version : ");

    Serial.println(FW_VERSION);

    Serial.print("Sketch Size : ");

    Serial.println(ESP.getSketchSize());

    Serial.print("Free Space : ");

    Serial.println(ESP.getFreeSketchSpace());

    Serial.print("Heap : ");

    Serial.println(ESP.getFreeHeap());

    Serial.println("==============================");
}



//------------------------------------------------------------
// Get OTA Status
//------------------------------------------------------------

OTAStatus getOTAStatus(void)
{
    return otaStatus;
}

//------------------------------------------------------------
// OTA Running?
//------------------------------------------------------------

bool otaInProgress(void)
{
    return otaRunning;
}

//------------------------------------------------------------
// OTA Progress
//------------------------------------------------------------

uint8_t getOTAProgress(void)
{
    return otaProgress;
}