#include "queue_manager.h"

//======================================================
// Ring Buffer
//======================================================

static  NodePacket queue[QUEUE_SIZE];

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static volatile uint8_t count = 0;

//======================================================
// Initialize Queue
//======================================================

void queueInit(void)
{
    noInterrupts();

    head = 0;
    tail = 0;
    count = 0;

    interrupts();
}

//======================================================
// Push Packet
//======================================================

bool queuePush(const NodePacket &packet)
{
    noInterrupts();

    // Queue Full
    if (count >= QUEUE_SIZE)
    {
        interrupts();
        return false;
    }

    queue[head] = packet;

    head++;

    if (head >= QUEUE_SIZE)
    {
        head = 0;
    }

    count++;

    interrupts();

    return true;
}

//======================================================
// Pop Packet
//======================================================

bool queuePop(NodePacket &packet)
{
    noInterrupts();

    // Queue Empty
    if (count == 0)
    {
        interrupts();
        return false;
    }

    packet = queue[tail];

    tail++;

    if (tail >= QUEUE_SIZE)
    {
        tail = 0;
    }

    count--;

    interrupts();

    return true;
}

//======================================================
// Queue Empty
//======================================================

bool queueIsEmpty(void)
{
    bool status;

    noInterrupts();
    status = (count == 0);
    interrupts();

    return status;
}

//======================================================
// Queue Full
//======================================================

bool queueIsFull(void)
{
    bool status;

    noInterrupts();
    status = (count == QUEUE_SIZE);
    interrupts();

    return status;
}

//======================================================
// Queue Count
//======================================================

uint8_t queueCount(void)
{
    uint8_t value;

    noInterrupts();
    value = count;
    interrupts();

    return value;
}

//======================================================
// Clear Queue
//======================================================

void queueClear(void)
{
    noInterrupts();

    head = 0;
    tail = 0;
    count = 0;

    interrupts();
}