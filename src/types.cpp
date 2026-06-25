#include "types.h"

const char* sensorTypeToString(uint8_t type)
{
    switch (type)
    {
        case SENSOR_DOOR:    return "door";
        case SENSOR_WINDOW:  return "window";
        case SENSOR_MOTION:  return "motion";
        case SENSOR_SMOKE:   return "smoke";
        case SENSOR_GAS:     return "gas";
        case SENSOR_PANIC:   return "panic";
        default:             return "unknown";
    }
}

const char* eventToString(uint8_t event)
{
    switch (event)
    {
        case EVENT_CLOSED:      return "closed";
        case EVENT_OPEN:        return "open";
        case EVENT_TRIGGERED:   return "triggered";
        case EVENT_HEARTBEAT:   return "heartbeat";
        case EVENT_LOW_BATTERY: return "low_battery";
        case EVENT_ONLINE:      return "online";
        case EVENT_OFFLINE:     return "offline";
        default:                return "unknown";
    }
}