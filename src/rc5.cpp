#include "mbed.h"
#include "include/config.h"
#include "include/rc5.h"
#include "include/move.h"
#include "include/outils.h"
#include "include/debug.h"

//#include <map>

namespace Telecommande {
    static InterruptIn signal {RC5};
    static Timer clock {};
    static const int long_pulse = usec(1778).count();
    static const int short_pulse = usec(889).count();
    static const int med_pulse = (long_pulse + short_pulse) / 2;
    static const usec max_long_pulse = 3000us; 
    static const uint8_t play_code = 53;

    static constexpr uint8_t COMMAND_LEN = 14;

    bool decoding = false;
    bool start_bit1 = false;
    bool start_bit2 = false;

    static uint16_t command = 0;
    uint8_t cur_bit;

    static inline void clock_restart() {
        clock.reset();
        clock.start();
    }

    bool good_startcode() {
        return ((command & (1 << 0)) && (command & (1 << 1)));
    }

    void decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (cur_bit >= COMMAND_LEN) {
            decoding = false;
            cur_bit = 0;
            clock.stop();
            clock.reset();

            if (good_startcode()) {
                if (((command >> 8) & 0xFF) == play_code) {
                    Move::mise_en_marche();
                }
            }
            command = 0;
            return;
        }

        clock_restart();
    }

    void decode_reset() {
        command = 0;
        decoding = true;
        clock_restart();
        cur_bit = 0;
    }

    void decode_fall() {
        if (!decoding || clock.elapsed_time() > max_long_pulse) {
            decode_reset();
            start_bit1 = true;
            return;    
        }

        if (GET_BIT(command, cur_bit) && clock.elapsed_time().count() < med_pulse) {
            clock_restart();
            return;
        }

        decode_bit(0);
    }

    void decode_rise() {
        if (!decoding) return;
        start_bit2 = true;
        if (cur_bit && !GET_BIT(command, cur_bit) && clock.elapsed_time().count() < med_pulse) {
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
        if (start_bit1) {
            DEBUG::print("Fall 1\r\n");
            start_bit1 = false;
        }
        if (start_bit2) {
            DEBUG::print("Rise\r\n");
            start_bit2 = false;
        }
    }
    
}
