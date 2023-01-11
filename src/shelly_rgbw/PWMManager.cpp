#include "PWMManager.h"

void PWMManager::set(const int& portNumber, const int& dimmerValue){

    LogUDP("set PWMManager");

    auto pin = this->PINS[portNumber];
    double dimValue = ((double)dimmerValue)*(double) 10.24;
    analogWrite(pin, int(dimValue));

}
