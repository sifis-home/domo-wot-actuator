#ifndef DOMO_ESP_POWERMEASUREMENTCONTROLLERADE7953_H
#define DOMO_ESP_POWERMEASUREMENTCONTROLLERADE7953_H

#include "DomoTimer.h"
#include "ADE7953_I2C.h"
#include "ShellyManager.h"


class PowerMeasurementControllerADE7953 {

public:

    PowerMeasurementControllerADE7953();

    void loop();

    void enableQuickPowerReport() {
        this->quickReportTimer.start();
    }

    double getPower1();
    double getPower2();

    double getEnergy1();
    double getEnergy2();

    double getSmoothedPower1() const;
    double getSmoothedPower2() const;

    ADE7953 ade7953 {};

    const ADE7953 &getAde7953() const;


protected:


    PowerData powerData;


    void _readPowerShellyEM();
    void _readPowerShelly25();

    DomoTimer measureSamplingTimer {};
    DomoTimer energySamplingTimer {};

    DomoTimer quickReportTimer {};

    DomoTimer measureReportTimer {};

    double energy1Sum {0};
    double energy2Sum {0};

    double lastPower1 {0};
    double lastPower2 {0};

    double smoothedPower1 {0};
    double smoothedPower2 {0};

    double powerOffset1 {0};
    double powerOffset2 {0};

    double lastUpdateTimestamp {-1};

    const double POWER_OFFSET_ALPHA {0.63};
    const double SMOOTHING_ALPHA {0.68};

};


#endif