#include "config.h"

#include <ESP8266WiFi.h>

//======================================================
// Device ID
//======================================================

String getDeviceID(void)
{
    String id = String(ESP.getChipId(), HEX);

    id.toUpperCase();

    return id;
}

//======================================================
// AP SSID
//======================================================

String getAPSSID(void)
{
    String id = getDeviceID();

    if (id.length() > 4)
    {
        id = id.substring(id.length() - 4);
    }

    return "SecureHome_" + id;
}

//======================================================
// AP Password
//======================================================

String getAPPassword(void)
{
    return "SH" + getDeviceID();
}