#ifndef AWS_MANAGER_H
#define AWS_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

bool initAWS(void);

BearSSL::WiFiClientSecure* getAWSClient(void);

#endif