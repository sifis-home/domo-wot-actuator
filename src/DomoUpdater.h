#ifndef DOMO_ESP_UPDATER_H
#define DOMO_ESP_UPDATER_H

#include <WString.h>
#include "DomoTimer.h"
#include "ArduinoJson.h"
#include "logging.h"

class DomoUpdater {
public:
    DomoUpdater(DomoUpdater &&) = delete;

    DomoUpdater &operator=(const DomoUpdater &) = delete;

    DomoUpdater &operator=(DomoUpdater &&) = delete;

    static DomoUpdater &getInstance() {
        static DomoUpdater instance;
        return instance;
    }


    DomoUpdater();

    bool update();

    void loop();

    void setFwToUpdate(const String& url);

    String currentFwVersion {""};

protected:

    String fwVersionToSet{""};
    DomoTimer timerUpdater;
    const uint32_t MAX_UPDATE_RETRIES{10};
    uint32_t updateRetries {0};

};


#endif
