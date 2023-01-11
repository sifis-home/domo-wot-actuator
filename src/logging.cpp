//
// Created by domenico on 9/5/22.
//

#include "logging.h"


void LogUDP(const String& s) {

    #ifdef SHELLY_DIMMER
    static WiFiUDP Udp;

    if (WiFi.isConnected()) {
        Udp.beginPacket(IPAddress(192, 168, 1, 49), 4389);
        Udp.write((s+"\n").c_str());
        Udp.endPacket();
    }
    #else
    Serial.println(s);
    Serial.flush();
    #endif

}