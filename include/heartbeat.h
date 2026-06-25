#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <Arduino.h>

// Initialize heartbeat module
void initHeartbeat(void);

// Call from loop()
void heartbeatTask(void);

#endif