#ifndef DOMO_ESP_PWMMANAGER_H
#define DOMO_ESP_PWMMANAGER_H

#include <Arduino.h>
#include "utils.h"
#include "../logging.h"

#define PWM_CHANNELS 4

class PWMManager {
public:

    void set(const int& portNumber, const int& dimmerValue);

    static PWMManager &getInstance() {
        static PWMManager instance;
        return instance;
    }

    void init() {

        LogUDP("init PWMManager");

        for(uint32_t i = 0; i < PWM_CHANNELS; ++i) {
            pinMode(this->PINS[i], OUTPUT);
            digitalWrite(this->PINS[i], 0);
        }

    }


protected:

    PWMManager(){}

    const uint8_t PINS[4] = {12, 15, 14, 4};

};

#endif
