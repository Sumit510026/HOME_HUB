/*
 * ============================================================
 * AWS TLS Manager
 * ============================================================
 *
 * Responsible only for:
 *  - Loading certificates
 *  - Configuring BearSSL
 *  - Providing secure client
 *
 * Does NOT:
 *  - Connect MQTT
 *  - Publish
 *  - Subscribe
 *
 * ============================================================
 */

#include "aws_manager.h"

#include <ESP8266WiFi.h>
#include <time.h>

#include "config.h"

#include "certificates/root_ca.h"
#include "certificates/cert.h"
#include "certificates/privateKey.h"

static BearSSL::WiFiClientSecure secureClient;

static BearSSL::X509List* rootCA = nullptr;
static BearSSL::X509List* deviceCert = nullptr;
static BearSSL::PrivateKey* privateKey = nullptr;

static bool syncTime();

bool initAWS()
{
    Serial.println("[AWS] Initializing TLS");

    if (!syncTime())
    {
        Serial.println("[AWS] NTP Failed");
        return false;
    }

    rootCA = new BearSSL::X509List(AWS_ROOT_CA);

    deviceCert = new BearSSL::X509List(AWS_CERT);

    privateKey = new BearSSL::PrivateKey(AWS_PRIVATE_KEY);

    secureClient.setTrustAnchors(rootCA);

    secureClient.setClientRSACert(deviceCert,
                                  privateKey);

    secureClient.setTimeout(30);

    Serial.println("[AWS] TLS Ready");

    return true;
}

BearSSL::WiFiClientSecure* getAWSClient()
{
    return &secureClient;
}

static bool syncTime()
{
    Serial.println("[AWS] Syncing Time");

    configTime(19800,
               0,
               "pool.ntp.org",
               "time.nist.gov",
               "time.google.com");

    uint32_t start = millis();

    time_t now = time(nullptr);

    while (now < 1700000000UL)
    {
        delay(500);

        Serial.print(".");

        if ((millis() - start) > 30000)
        {
            Serial.println();

            return false;
        }

        now = time(nullptr);
    }

    Serial.println();

    Serial.print("[AWS] Time : ");

    Serial.print(ctime(&now));

    return true;
}