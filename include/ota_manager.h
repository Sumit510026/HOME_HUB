#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>

/*
 * ============================================================
 * OTA Manager
 * ============================================================
 *
 * Responsibilities
 * ----------------
 * - Perform HTTPS firmware update
 * - Compare firmware versions
 * - Report OTA state
 * - Track OTA progress
 *
 * Does NOT
 * --------
 * - Receive MQTT commands
 * - Connect WiFi
 * - Connect AWS
 *
 * ============================================================
 */

//---------------------------------------------------------------
// Current firmware version
//---------------------------------------------------------------

#define FW_VERSION      "1.0.0"

//---------------------------------------------------------------
// OTA Status
//---------------------------------------------------------------

typedef enum
{
    OTA_IDLE = 0,

    OTA_DOWNLOADING,

    OTA_INSTALLING,

    OTA_SUCCESS,

    OTA_FAILED

} OTAStatus;

//---------------------------------------------------------------
// Initialization
//---------------------------------------------------------------

bool initOTA(void);

//---------------------------------------------------------------
// Call from loop()
// (Reserved for future background tasks)
//---------------------------------------------------------------

void otaTask(void);

//---------------------------------------------------------------
// Start OTA Update
//---------------------------------------------------------------

bool startOTA(const char *firmwareURL,
              const char *newVersion);

//---------------------------------------------------------------
// Status
//---------------------------------------------------------------

OTAStatus getOTAStatus(void);

bool otaInProgress(void);

//---------------------------------------------------------------
// Version
//---------------------------------------------------------------

const char *getFirmwareVersion(void);

bool isNewVersionAvailable(const char *newVersion);

//---------------------------------------------------------------
// Progress
//---------------------------------------------------------------

uint8_t getOTAProgress(void);

#endif