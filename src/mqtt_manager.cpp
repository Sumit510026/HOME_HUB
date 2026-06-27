/*
 * ============================================================
 * MQTT Manager
 * Home Security Hub
 * ============================================================
 *
 * Responsibilities
 * ----------------
 * - MQTT Connection
 * - Automatic Reconnection
 * - Publish Events
 * - Publish Heartbeat
 * - Receive Commands
 *
 * ============================================================
 */

#include "mqtt_manager.h"

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "aws_manager.h"
#include "wifi_manager.h"
#include "queue_manager.h"
#include "ota_manager.h"
#include "espnow_manager.h"
#include "config.h"

//============================================================
// Globals
//============================================================

static BearSSL::WiFiClientSecure *awsClient = nullptr;

static PubSubClient mqtt;

//============================================================

static uint32_t lastReconnectAttempt = 0;

static uint32_t reconnectCount = 0;

//============================================================
// Forward Declarations
//============================================================

static void mqttCallback(char *topic,
                         byte *payload,
                         unsigned int length);

static void printTLSStatus(void);

static bool subscribeTopics(void);

//============================================================
// Initialize MQTT
//============================================================

bool initMQTT(void)
{
    awsClient = getAWSClient();

    mqtt.setClient(*awsClient);

    mqtt.setServer(AWS_ENDPOINT,
                   AWS_PORT);

    mqtt.setCallback(mqttCallback);

    mqtt.setKeepAlive(MQTT_KEEPALIVE);

    mqtt.setSocketTimeout(30);

    mqtt.setBufferSize(MQTT_BUFFER_SIZE);

    Serial.println("[MQTT] Initialized");

    return mqttReconnect();
}

//============================================================
// Reconnect
//============================================================

bool mqttReconnect(void)
{
    if (!wifiConnected())
    {
        Serial.println("[MQTT] WiFi not connected");
        return false;
    }

    if (mqtt.connected())
        return true;

    //--------------------------------------------------------
    // Close any stale TLS socket
    //--------------------------------------------------------

    awsClient->stop();

    delay(100);

    //--------------------------------------------------------
    // Client ID
    //--------------------------------------------------------

    char clientID[40];

    snprintf(clientID,
             sizeof(clientID),
             "%s_%06X",
             MQTT_CLIENT_ID,
             ESP.getChipId());

    //--------------------------------------------------------

    Serial.println();
    Serial.println("====================================");
    Serial.println("[MQTT] Connecting to AWS IoT");
    Serial.println("====================================");

    Serial.print("[MQTT] Client ID : ");
    Serial.println(clientID);

    Serial.print("[MQTT] Free Heap : ");
    Serial.println(ESP.getFreeHeap());

    //--------------------------------------------------------

    bool connected = mqtt.connect(clientID);

    if (!connected)
    {
        reconnectCount++;

        Serial.printf("[MQTT] Connect Failed (%d)\n",
                      mqtt.state());

        printTLSStatus();

        return false;
    }

    Serial.println("[MQTT] Connected");

    // Serial.printf("[MQTT] state=%d\n", mqtt.state());

    // delay(2000);

    // Serial.printf("[MQTT] state after 2 sec=%d\n", mqtt.state());

    reconnectCount = 0;

    if (!subscribeTopics())
    {
        Serial.println("[MQTT] Subscribe Failed");
    }
    else
    {
        Serial.println("[MQTT] Subscribed");
    }

    //mqttPublishStatus();

    return true;
}

//============================================================
// Subscribe Topics
//============================================================

static bool subscribeTopics(void)
{
    return mqtt.subscribe(MQTT_TOPIC_COMMAND);
}

//============================================================
// TLS Diagnostics
//============================================================

static void printTLSStatus(void)
{
    Serial.println();
    Serial.println("----- TLS Diagnostics -----");

    Serial.printf("WiFi Status : %d\n",
                  WiFi.status());

    Serial.printf("TCP Socket  : %d\n",
                  awsClient->connected());

    Serial.printf("MQTT State  : %d\n",
                  mqtt.state());

    Serial.printf("RSSI        : %d dBm\n",
                  WiFi.RSSI());

    Serial.printf("Heap        : %u\n",
                  ESP.getFreeHeap());

    char err[128];

    int sslErr =
        awsClient->getLastSSLError(err,
                                   sizeof(err));

    Serial.printf("SSL Error   : %d\n",
                  sslErr);

    Serial.print("SSL Reason  : ");

    if (strlen(err))
        Serial.println(err);
    else
        Serial.println("None");

    Serial.println("----------------------------");
}

//============================================================
// MQTT Task
//============================================================

void mqttTask(void)
{
    //--------------------------------------------------------
    // WiFi Required
    //--------------------------------------------------------

    if (!wifiConnected())
        return;

    //--------------------------------------------------------
    // MQTT Reconnect
    //--------------------------------------------------------

    if (!mqtt.connected())
    {
        uint32_t now = millis();

        if ((now - lastReconnectAttempt) >= MQTT_RECONNECT_INTERVAL)
        {
            lastReconnectAttempt = now;

            Serial.printf("[MQTT] Disconnected (State=%d)\n",
                          mqtt.state());

            printTLSStatus();

            mqttReconnect();
        }

        return;
    }

    //--------------------------------------------------------
    // Keep MQTT Alive
    //--------------------------------------------------------
bool ok = mqtt.loop();

if (!ok)
{
    Serial.printf("[MQTT] loop() failed, state=%d\n", mqtt.state());
    Serial.printf("[MQTT] TCP=%d MQTT=%d\n",
                  awsClient->connected(),
                  mqtt.connected());

    printTLSStatus();
    return;
}

    //--------------------------------------------------------
    // Publish queued sensor packets
    //--------------------------------------------------------

    NodePacket packet;

    while (queuePop(packet))
    {
        if (!mqttPublishEvent(packet))
        {
            Serial.println("[MQTT] Failed to publish event");

            break;
        }
    }
}

//============================================================
// Publish Sensor Event
//============================================================

bool mqttPublishEvent(const NodePacket &pkt)
{
    if (!mqtt.connected())
        return false;

    StaticJsonDocument<256> doc;

    doc["nodeID"]   = pkt.nodeID;
    doc["sensor"]   = sensorTypeToString(pkt.sensorType);
    doc["event"]    = eventToString(pkt.event);
    doc["battery"]  = pkt.battery;
    doc["sequence"] = pkt.sequence;
    doc["uptime"]   = pkt.uptime;

    doc["hubRSSI"]  = wifiRSSI();
    doc["heap"]     = ESP.getFreeHeap();

    char payload[256];

    size_t len =
        serializeJson(doc,
                      payload,
                      sizeof(payload));

    char topic[64];

    snprintf(topic,
             sizeof(topic),
             "%s/node%d",
             MQTT_TOPIC_EVENT,
             pkt.nodeID);

    bool ok =
        mqtt.publish(topic,
                     payload);

    if (!ok)
    {
        Serial.println("[MQTT] Publish Failed");
        printTLSStatus();
    }

    return ok;
}

//============================================================
// Publish Heartbeat
//============================================================
/* //origiral fxn

bool mqttPublishHeartbeat(void)
{
    StaticJsonDocument<256> doc;

    doc["device"] = MQTT_CLIENT_ID;
    doc["status"] = "online";
    doc["uptime"] = millis() / 1000;

    doc["heap"] = ESP.getFreeHeap();
    doc["rssi"] = wifiRSSI();

    doc["queue"] = queueCount();

    doc["ip"] = wifiIP();
    doc["mac"] = wifiMAC();

    doc["firmware"] = FW_VERSION;

    char payload[256];

    size_t len =
        serializeJson(doc,
                      payload,
                      sizeof(payload));

                    

    return mqtt.publish(
            MQTT_TOPIC_HEARTBEAT,
            payload,
            true);
}  */


//temerary fxn to publish heartbeat and status
bool mqttPublishHeartbeat(void)
{
    StaticJsonDocument<256> doc;

    doc["device"]   = MQTT_CLIENT_ID;
    doc["status"]   = "online";
    doc["uptime"]   = millis() / 1000;
    doc["heap"]     = ESP.getFreeHeap();
    doc["rssi"]     = wifiRSSI();
    doc["queue"]    = queueCount();
    doc["ip"]       = wifiIP();
    doc["mac"]      = wifiMAC();
    doc["firmware"] = FW_VERSION;

    char payload[256];

    size_t len = serializeJson(doc, payload, sizeof(payload));

    Serial.printf("[Heartbeat] JSON Size = %u\n", len);
    Serial.println(payload);

    bool ok = mqtt.publish(MQTT_TOPIC_HEARTBEAT, payload); // remove true to make it non-retained

    Serial.printf("[Heartbeat] Publish returned = %d\n", ok);
    Serial.printf("[Heartbeat] MQTT state = %d\n", mqtt.state());

    return ok;
}



//============================================================
// Publish Status
//============================================================

bool mqttPublishStatus(void)
{
    return mqttPublishHeartbeat();
}

//============================================================
// Generic Publish
//============================================================

bool mqttPublish(const char *topic,
                 const char *payload,
                 bool retained)
{
    if (!mqtt.connected())
        return false;

    bool ok =
        mqtt.publish(topic,
                     payload,
                     retained);

    if (!ok)
    {
        Serial.print("[MQTT] Publish Failed : ");
        Serial.println(topic);

        printTLSStatus();
    }

    return ok;
}

//============================================================
// Publish JSON
//============================================================

bool mqttPublishJSON(const char *topic,
                     JsonDocument &doc,
                     bool retained)
{
    char payload[512];

    serializeJson(doc,
                  payload,
                  sizeof(payload));

    return mqttPublish(topic,
                       payload,
                       retained);
}

//============================================================
// MQTT Connected
//============================================================

bool mqttConnected(void)
{
    return mqtt.connected();
}

//============================================================
// MQTT Callback
//============================================================

static void mqttCallback(char *topic,
                         byte *payload,
                         unsigned int length)
{
    Serial.println();
    Serial.println("====================================");
    Serial.println("[MQTT] Command Received");
    Serial.println("====================================");

    Serial.print("[MQTT] Topic : ");
    Serial.println(topic);

    //--------------------------------------------------------
    // Parse JSON
    //--------------------------------------------------------

    StaticJsonDocument<256> doc;

    DeserializationError err =
        deserializeJson(doc, payload, length);

    if (err)
    {
        Serial.print("[MQTT] JSON Error : ");
        Serial.println(err.c_str());
        return;
    }

    const char *cmd = doc["cmd"] | "";

    Serial.print("[MQTT] Command : ");
    Serial.println(cmd);

    //--------------------------------------------------------
    // STATUS
    //--------------------------------------------------------

    if (strcmp(cmd, "status") == 0)
    {
        Serial.println("[MQTT] Publishing Status");

       // mqttPublishStatus();

        return;
    }

    //--------------------------------------------------------
    // REBOOT
    //--------------------------------------------------------

    if (strcmp(cmd, "reboot") == 0)
    {
        Serial.println("[MQTT] Reboot Requested");

        delay(500);

        ESP.restart();

        return;
    }

    //--------------------------------------------------------
    // OTA UPDATE
    //--------------------------------------------------------

    if (strcmp(cmd, "ota") == 0)
    {
        const char *url =
            doc["url"] | "";

        const char *version =
            doc["version"] | "";

        if (strlen(url) == 0 ||
            strlen(version) == 0)
        {
            Serial.println("[MQTT] Invalid OTA Command");
            return;
        }

        Serial.println("[MQTT] Starting OTA");

        startOTA(url,
                 version);

        return;
    }

    //--------------------------------------------------------
    // ARM
    //--------------------------------------------------------

    if (strcmp(cmd, "arm") == 0)
    {
        Serial.println("[MQTT] ARM command");

        // TODO:
        // Arm security system

        return;
    }

    //--------------------------------------------------------
    // DISARM
    //--------------------------------------------------------

    if (strcmp(cmd, "disarm") == 0)
    {
        Serial.println("[MQTT] DISARM command");

        // TODO:
        // Disarm security system

        return;
    }

    //--------------------------------------------------------
    // PING
    //--------------------------------------------------------

    if (strcmp(cmd, "ping") == 0)
    {
        StaticJsonDocument<128> reply;

        reply["device"] = MQTT_CLIENT_ID;
        reply["status"] = "alive";
        reply["uptime"] = millis() / 1000;
        reply["heap"] = ESP.getFreeHeap();
        reply["rssi"] = wifiRSSI();

        mqttPublishJSON(MQTT_TOPIC_STATUS,
                        reply,
                        false);

        return;
    }

    //--------------------------------------------------------
    // UNKNOWN COMMAND
    //--------------------------------------------------------

    Serial.print("[MQTT] Unknown Command : ");
    Serial.println(cmd);
}