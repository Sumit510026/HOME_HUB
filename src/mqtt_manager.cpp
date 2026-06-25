#include "mqtt_manager.h"
#include "ota_manager.h"

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "aws_manager.h"
#include "wifi_manager.h"
#include "queue_manager.h"
#include "config.h"

static PubSubClient mqtt(*getAWSClient());

static uint32_t lastReconnect = 0;

static void mqttCallback(char *topic,
                         byte *payload,
                         unsigned int length);

bool initMQTT()
{
    mqtt.setServer(AWS_ENDPOINT,
                   AWS_PORT);

    mqtt.setCallback(mqttCallback);

    mqtt.setKeepAlive(MQTT_KEEPALIVE);

    mqtt.setBufferSize(MQTT_BUFFER_SIZE);

    return mqttReconnect();
}

bool mqttReconnect()
{
    if (!wifiConnected())
        return false;

    Serial.println("[MQTT] Connecting...");

   char clientID[32];

    snprintf(clientID,
         sizeof(clientID),
         "%s_%06X",
         MQTT_CLIENT_ID,
         ESP.getChipId());

    StaticJsonDocument<64> doc;

    doc["status"] = "offline",doc["device"] = "HOME_HUB";

    char lwt[64];

    serializeJson(doc, lwt);

    bool ok = mqtt.connect(clientID,
                           NULL,
                           NULL,
                           MQTT_TOPIC_STATUS,
                           1,
                           true,
                           lwt);

    if (!ok)
    {
        Serial.print("[MQTT] Failed : ");

        Serial.printf("[MQTT] Connect Failed (%d)\n",
              mqtt.state());

        return false;
    }

    if (!mqtt.subscribe(MQTT_TOPIC_COMMAND))
    {
    Serial.println("[MQTT] Subscribe Failed");
    }

    Serial.println("[MQTT] Connected");
    mqttPublishStatus();
    return true;
}

void mqttTask()
{
    if (!mqtt.connected())
    {
        if (millis() - lastReconnect > MQTT_RECONNECT_INTERVAL)
        {
            lastReconnect = millis();

            mqttReconnect();
        }

        return;
    }

    mqtt.loop();

    NodePacket packet;

   while (queuePop(packet))
    {
    if (!mqttPublishEvent(packet))
    {
        Serial.println("[MQTT] Publish Failed");
        break;
    }
    }

    
}

bool mqttPublishEvent(const NodePacket &pkt)
{
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

    serializeJson(doc, payload, sizeof(payload));

    char topic[64];

    snprintf(topic,
             sizeof(topic),
             "%s/node%d",
             MQTT_TOPIC_EVENT,
             pkt.nodeID);

    return mqtt.publish(topic, payload);
}

bool mqttPublishHeartbeat()
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

    doc["firmware"] = "1.0.0";
    doc["version"] = 1;

    char payload[256];

    serializeJson(doc, payload);

    return mqtt.publish(MQTT_TOPIC_HEARTBEAT,
                        payload,
                        true);
}

static void mqttCallback(char *topic,
                         byte *payload,
                         unsigned int length)
{
    StaticJsonDocument<128> doc;

    DeserializationError err =
        deserializeJson(doc, payload, length);

    if (err)
        return;

    const char *cmd = doc["cmd"] | "";

    if (strcmp(cmd, "status") == 0)
{
    mqttPublishStatus();
}
else if (strcmp(cmd, "reboot") == 0)
{
    ESP.restart();
}
else if (strcmp(cmd, "ota") == 0)
{
    const char *url = doc["url"] | "";
    const char *version = doc["version"] | "";

    if (strlen(url) == 0 || strlen(version) == 0)
    {
        Serial.println("[MQTT] Invalid OTA Command");
        return;
    }

    startOTA(url, version);
}
else if (strcmp(cmd, "arm") == 0)
{
    // Future
}
else if (strcmp(cmd, "disarm") == 0)
{
    // Future
}
}

bool mqttPublishStatus()
{
    return mqttPublishHeartbeat();
}

bool mqttPublish(const char *topic,
                 const char *payload,
                 bool retained)
{
    return mqtt.publish(topic,
                        payload,
                        retained);
}


bool mqttPublishJSON(const char *topic,
                     JsonDocument &doc,
                     bool retained)
{
    char payload[512];

    serializeJson(doc, payload);

    return mqtt.publish(topic,
                        payload,
                        retained);
}

bool mqttConnected(void)
{
    return mqtt.connected();
}