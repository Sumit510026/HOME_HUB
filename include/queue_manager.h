#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include <Arduino.h>
#include "types.h"

#define QUEUE_SIZE 20

void queueInit(void);

bool queuePush(const NodePacket &packet);

bool queuePop(NodePacket &packet);

bool queueIsEmpty(void);

bool queueIsFull(void);

uint8_t queueCount(void);

#endif