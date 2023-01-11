#include "cap_shelly_dimmer.h"
#include "utils.h"
#include "logging.h"


// will be called in loop, if you return true here, every else will be skipped !!
// so you CAN run uninterrupted by returning true, but you shouldn't do that for
// a long time, otherwise nothing else will be executed
bool shelly_dimmer::loop() {
    // state machine to parse incoming data
    if (Serial.available()) {

        t = Serial.read();

        if (millis() - m_last_char_recv > 600) { // typically we have 1989 ms between last char of a status request and first ..
            m_recv_state = SHD_START;
        }

        m_last_char_recv = millis();

//        ("Received %02x, state %i\r\n",t,m_recv_state);
        if (m_recv_state == SHD_START) {
            if (t == 0x01) { // start byte
                //logger.pln("s");
                LogUDP("Received Start Byte");
                m_msg_in.start = t;
                m_recv_state++;
                m_msg_in.chk = t;
            }
        } else if (m_recv_state == SHD_ID) {
            m_msg_in.id = t;
            m_recv_state++;
            m_msg_in.chk += t;
        } else if (m_recv_state == SHD_CMD) {
            //logger.pln("v");
            m_msg_in.cmd = t;
            m_recv_state++;
            m_msg_in.chk += t;
        } else if (m_recv_state == SHD_LEN) {
            //logger.pln("cmd");
            m_msg_in.len = t;
            m_msg_in.data_p = 0;
            m_recv_state++; // data ..
            if (m_msg_in.len == 0) {
                m_recv_state++; // if no data .. goto chk
            }
            m_msg_in.chk += t;
        } else if (m_recv_state == SHD_DATA) {
            //logger.pln("len");
            m_msg_in.data[m_msg_in.data_p] = t;
            m_msg_in.data_p++;
            //Serial.printf("recv %i / %i \r\n",m_msg_in.data_p,m_msg_in.len);
            m_msg_in.chk += t;
            if (m_msg_in.data_p > sizeof(m_msg_in.data) - 1) {
                m_msg_in.data_p = 0;
            }
            if (m_msg_in.data_p == m_msg_in.len) {
                m_recv_state++;
            }
        } else if (m_recv_state == SHD_CHK_1) {
            //logger.pln("chk1");
            m_msg_in.chk -= 1;
            if ((m_msg_in.chk >> 8) == t) {
                //logger.pln("ok");
                m_recv_state++;
            } else {
               LogUDP("Bad Checksum 1");
                m_recv_state = SHD_START;
            }
        } else if (m_recv_state == SHD_CHK_2) {
            //logger.pln("chk2");
            LogUDP("SHD_CHK2");
            if ((m_msg_in.chk & 0xff) == t) {


                if (m_msg_in.cmd == 0x10) {

                    /*
                    uint32_t power_raw = (((uint32_t) m_msg_in.data[7]) << 24) + (((uint32_t) m_msg_in.data[6]) << 16) +
                                         (((uint16_t) m_msg_in.data[5]) << 8) + m_msg_in.data[4];
                                        */

                    uint32_t power_raw =  ((((uint32_t) m_msg_in.data[7]) << 8u ) + (uint32_t) m_msg_in.data[6]) / 20;


                    if (this->cb) {
                        this->cb->powerMeasured(power_raw);
                    }

//                    this->m_power = (uint16_t) (1000000UL / power_raw);



                } else if (this->cb && m_msg_in.cmd == 0x02 ) {
                    // no need to see all dimming messages, they will just flood the console when we dimm ourself
                    LogUDP("DIM COMPLETE FROM SERIAL");
                    this->cb->lastDimComplete();
                }
            }
            m_recv_state = SHD_START;
        } else {

        }
        //Serial.printf("Next state %i\r\n",m_recv_state);
    } else if (millis() - m_last_char_recv > SHELLY_DIMMER_POWER_UPDATE_RATE) { // 10 sec update

        LogUDP("Requesting power");

        send_cmd(0x10, 0, this->buffer);
        m_last_char_recv = millis(); // avoid resend
    }
    return false; // i did nothing that shouldn't be non-interrupted
}


void shelly_dimmer::set_dim(uint32_t value) {

    uint16_t r_scale = value * 10;

    uint8_t data[2];

    data[0] = r_scale & 0xff;
    data[1] = r_scale >> 8;

    //send_cmd(1, 2, data);

    send_cmd(0x02, 2, data);

    LogUDP("Dimming command sent");

}


void shelly_dimmer::send_cmd(uint8_t cmd, uint8_t len, uint8_t *payload) {
    char data[len + 7];
    uint16_t chk = -1;
    data[0] = 0x01;                // fix
    data[1] = m_msg_in.id + 1;        // id
    data[2] = cmd;                // cmd e.g. 0x01= set brightness
    data[3] = len;                // len
    for (uint8_t i = 0; i < len; i++) {
        data[4 + i] = payload[i];
    }
    // calc chk
    for (uint8_t i = 0; i < 4 + len; i++) {
        chk += data[i];
    }
    data[len + 4] = chk >> 8;        // high nibble
    data[len + 5] = chk & 0xff;        // low nibble
    data[len + 6] = 0x04;            // fix
    Serial.write(data, len + 7);

}

void shelly_dimmer::set_dim_faded(uint32_t value, uint32_t speed_rate, uint8_t func) {

    uint16_t r_scale = value * 10;

    uint8_t data[6];

    data[0] = r_scale & 0xff;
    data[1] = r_scale >> 8;
    data[2] = func & 0xff;;
    data[3] = 0x00;
    data[4] = speed_rate & 0xff;
    data[5] = speed_rate >> 8;

    send_cmd(0x20, 6, data);

    LogUDP("Faded Dimming command sent");
}

shelly_dimmer::shelly_dimmer(shelly_dimmer::callback *callback) : cb(callback) {

}

