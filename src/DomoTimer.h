#ifndef DOMO_ESP_DOMOTIMER_H
#define DOMO_ESP_DOMOTIMER_H

#include <cstdint>
#include <Arduino.h>

class DomoTimer {
protected:
    unsigned long startMillis;
    bool running {true};
public:

    explicit DomoTimer(bool startEnabled = true);

    void reset();
    void stop();

    void start();

    bool isRunning() const;

    bool elapsed(unsigned long ms,  bool autoreset=true);

};


#endif
