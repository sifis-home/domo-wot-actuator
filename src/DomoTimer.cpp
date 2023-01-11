#include "DomoTimer.h"

DomoTimer::DomoTimer(bool startEnabled) : startMillis(millis()), running(startEnabled) {

}

void DomoTimer::reset() {
    startMillis = millis();
}

bool DomoTimer::elapsed(unsigned long ms, bool autoreset) {
    if (!this->running)
        return true;

    auto cond = (bool) (((unsigned long) (millis() - startMillis)) > ms);
    if (cond && autoreset)
        this->reset();
    return cond;
}

void DomoTimer::stop() {
    this->running = false;

}

void DomoTimer::start() {
    this->reset();
    this->running = true;
}

bool DomoTimer::isRunning() const {
    return running;
}
