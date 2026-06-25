#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#pragma pack(push,1)

typedef struct
{
    uint8_t nodeID;

    uint8_t sensorType;

    uint8_t event;

    uint8_t battery;

    uint16_t sequence;

    uint32_t uptime;

    uint16_t crc;

} NodePacket;

#pragma pack(pop)

enum SensorType
{
    SENSOR_DOOR = 1,
    SENSOR_WINDOW,
    SENSOR_MOTION,
    SENSOR_SMOKE,
    SENSOR_GAS,
    SENSOR_PANIC
};

enum EventType
{
    EVENT_CLOSED = 0,
    EVENT_OPEN,
    EVENT_TRIGGERED,
    EVENT_HEARTBEAT,
    EVENT_LOW_BATTERY,
    EVENT_ONLINE,
    EVENT_OFFLINE
};

const char* sensorTypeToString(uint8_t type);
const char* eventToString(uint8_t event);

#endif