#include "mbed.h"
#include "include/config.h"
#include "include/rc5.h"
#include "include/move.h"
#include "include/outils.h"

namespace Telecommande {
    static InterruptIn signal {RC5};
    static Timer clock {};
    static const int long_pulse = usec(1778).count();
    static const int short_pulse = usec(889).count();
    static const int med_pulse = (long_pulse + short_pulse) / 2;

    bool decoding = false;
    bool decoded = false;
    bool start_bit1 = false;
    bool start_bit2 = false;

    static uint8_t command[14];
    static uint8_t play_code = 53;
    uint8_t cur_bit_num;
    bool prev_bit;

    static inline void clock_restart() {
        clock.reset();
        clock.start();
    }

    bool check_good_startcode() {
        return (command[0] && command[1]);
    }

    uint8_t binarr_to_num(uint8_t *binarr) {
        uint8_t res = 0;

        for (int i = 0; i < 6; i++) {
            res = (binarr[i] << (5 - i)) | res;
        }
        return res;
    }

    void decode_bit(uint8_t bit) {
        command[cur_bit_num] = bit;
        cur_bit_num++;

        if (cur_bit_num >= ARR_SIZE(command)) {
            clock.stop();
            decoding = false;

            if (check_good_startcode()) {
                if (binarr_to_num(command + 8) == play_code) {
                    Move::marche();
                }
            }

            memset(command, 0, sizeof(command));
        }

        clock_restart();
    }

    void decode_fall() {
        if (!decoding) {
            decoding = true;
            clock_restart();
            cur_bit_num = 0;
            return;
        }

        if (command[cur_bit_num] && clock.elapse_time().count() < med_time) {
            clock_restart();
            return;
        }

        decode_bit(0);
    }

    void decode_rise() {
        if (!decoding) return;

        if (!command[cur_bit_num] && clock.elapsed_time().count() < med_time) {
            clock_restart();
            return;
        }

        decode_bit(1);
    }

    void init() {
        signal.mode(PullNone);
        signal.rise(&decode_rise);
        signal.fall(&decode_fall);
    }

    void control() {
        /*if (decoded) {
            if (command)
            decoded = false;
        }*/
    }
}