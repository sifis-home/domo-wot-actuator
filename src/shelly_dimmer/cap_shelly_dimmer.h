#ifndef shelly_dimmer_h
#define shelly_dimmer_h

#include "Arduino.h"

#define SHELLY_DIMMER_POWER_UPDATE_RATE 30000UL // 30 sec

#define SHD_START   10
#define SHD_ID      11
#define SHD_CMD     12
#define SHD_LEN     13
#define SHD_DATA    14
#define SHD_CHK_1   15
#define SHD_CHK_2   16
#define SHD_END     17

struct shelly_dimmer_msg {
    uint8_t start;
    uint8_t id;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[72];
    uint8_t data_p;
    uint16_t chk;
    uint8_t end;
};


class shelly_dimmer {

public:
    struct callback {
        virtual void powerMeasured(double watt) = 0;

        virtual void lastDimComplete() = 0;
    };

    shelly_dimmer(shelly_dimmer::callback *callback);

    ~shelly_dimmer() = default;

    bool init();

    bool loop();

    // light providing capability specific calls
    void set_dim(uint32_t value);

    void set_dim_faded(uint32_t value, uint32_t speed_rate, uint8_t func);

private:

    void send_cmd(uint8_t cmd, uint8_t len, uint8_t *payload);

    uint8_t m_recv_state{SHD_START};

    shelly_dimmer_msg m_msg_in{};

    uint8_t t{}; // temporary serial byte
    uint8_t buffer[256];

    callback *cb{nullptr};

    uint32_t m_power{0};
    uint32_t m_last_char_recv{0};
};


#endif
