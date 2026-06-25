/*
 * ============================================================
 * ESP-NOW Manager
 * Home Security Hub
 * ============================================================
 */

#include "espnow_manager.h"

#include <ESP8266WiFi.h>

extern "C"
{
#include <espnow.h>
}

#include "types.h"
#include "queue_manager.h"

//====================================================
// Statistics
//====================================================

static volatile uint32_t totalReceived = 0;
static volatile uint32_t totalDropped  = 0;
static volatile uint32_t invalidPacket = 0;

//====================================================
// Last Sequence Number
// Used for duplicate packet detection
//====================================================

#define MAX_NODE_COUNT 32

static uint16_t lastSequence[MAX_NODE_COUNT];

//====================================================

static void onDataReceive(uint8_t *mac,
                          uint8_t *incomingData,
                          uint8_t len);

//====================================================

bool initESPNow(void)
{
    Serial.println();
    Serial.println("[ESP-NOW] Initializing...");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0)
    {
        Serial.println("[ESP-NOW] Initialization Failed");

        return false;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

    esp_now_register_recv_cb(onDataReceive);

    memset(lastSequence,
           0,
           sizeof(lastSequence));

    Serial.println("[ESP-NOW] Ready");

    Serial.print("[ESP-NOW] Hub MAC : ");

    Serial.println(WiFi.macAddress());

    return true;
}

//====================================================
// Receive Callback
//====================================================

static void onDataReceive(uint8_t *mac,
                          uint8_t *incomingData,
                          uint8_t len)
{
    if (len != sizeof(NodePacket))
    {
        invalidPacket++;

        return;
    }

    NodePacket packet;

    memcpy(&packet,
           incomingData,
           sizeof(NodePacket));

    // Invalid Node ID
    if (packet.nodeID == 0 ||
        packet.nodeID > MAX_NODE_COUNT)
    {
        invalidPacket++;

        return;
    }

    // Duplicate packet detection
    if (packet.sequence ==
        lastSequence[packet.nodeID - 1])
    {
        return;
    }

    lastSequence[packet.nodeID - 1] =
        packet.sequence;

    totalReceived++;

    if (!queuePush(packet))
    {
        totalDropped++;
    }
}

//====================================================

void espnowTask(void)
{
    /*
     * Reserved
     *
     * Future:
     * - ACK handling
     * - Peer management
     * - Diagnostics
     */
}

//====================================================

uint32_t espnowPacketsReceived(void)
{
    return totalReceived;
}

uint32_t espnowPacketsDropped(void)
{
    return totalDropped;
}

uint32_t espnowInvalidPackets(void)
{
    return invalidPacket;
}