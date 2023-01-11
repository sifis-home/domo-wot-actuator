#ifndef DOMO_ESP_STM32UPDATER_H
#define DOMO_ESP_STM32UPDATER_H

#include "Arduino.h"
#include "FS.h"
#include "STM32Utils.h"
#include "stm32flash.h"

class STM32Updater {

public:

    bool updateSTM32();
    ~STM32Updater();

protected:

    bool downloadFW();

    void reset();
    void resetToDFUMode();

    bool flashUpload(File& fwFile);
    bool flashBegin();
    void flashEnd();

    // For uploading the STM32 firmware
    stm32_t  *stm32 {nullptr};
    uint32_t  stm32Addr {0};


};


#endif
