#include "mbed.h"
#include "include/config.h"
#include "include/rc5.h"
#include "include/move.h"
#include "include/outils.h"
#include "include/debug.h"

//#include <map>

namespace Telecommande {

    static InterruptIn signal {RC5_pin};
    static Timer clock {};

    bool bootstrapped = false;
    bool decoding = false;
    bool decoded = false;
    bool start_bit1 = false;
    bool start_bit2 = false;

    static uint16_t command = 0;
    int cur_bit;

    static inline void clock_restart() {
        clock.reset();
        //clock.start();
    }

    static void decode_reset() {
        command = 0;
        decoding = true;
        clock.reset();
        clock.start();
        cur_bit = 0;
    }

namespace RC5 {
    static const int long_pulse = usec(1778).count();
    static const int short_pulse = usec(889).count();
    static const int med_pulse = (long_pulse + short_pulse) / 2;
    static const usec max_long_pulse = 3000us; 
    static const uint8_t play_code = 53;

    static constexpr int COMMAND_LEN = 14;

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
            decoded = true;
            cur_bit = 0;
            clock.stop();
            clock.reset();

            if (good_startcode()) {
                if (((command >> 8) & 0xFF) == play_code) {
                    Move::mise_en_marche();
                }
            }
            return;
        }

        clock_restart();
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

        //start_bit1 = true;
        decode_bit(0);
    }

    void decode_rise() {
        if (!decoding) return;

        if (cur_bit && !GET_BIT(command, cur_bit) && clock.elapsed_time().count() < med_pulse) {
            clock_restart();
            return;
        }

        start_bit2 = true;
        decode_bit(1);
    }
}

namespace SIRC {
    static constexpr int COMMAND_LEN = 12;
    static constexpr usec start_bit_pulse = 2000us;
    static constexpr usec high_pulse = 1600us;
    static constexpr usec low_pulse = 1000us;

    uint8_t play_code = 24;

    bool good_startcode() {
        return command & (1 << 0);
    }

    void decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (cur_bit >= COMMAND_LEN) {
            decoding = false;
            decoded = true;
            cur_bit = 0;
            clock.stop();
            clock.reset();

            if (good_startcode()) {
                if (((command >> 1) & 0x7F) == play_code) {
                    Move::mise_en_marche();
                }
            }
            return;
        }

        clock_restart();
    }

    void decode_fall() {
        if (!decoding || clock.elapsed_time() > start_bit_pulse * 2) {
            decode_reset();
            start_bit1 = true;
            return;    
        }

        clock_restart();
    }

    void decode_rise() {
        if (!decoding) return;

        if (clock.elapsed_time() > high_pulse) {
            decode_bit(1);
        } else if (clock.elapsed_time() > low_pulse) {
            decode_bit(0);
        }
    }
}

    void bootstrap() {
        if (bootstrapped) return;
        bootstrapped = true;
        Move::mise_en_marche();
    }

    void init() {
        signal.mode(PullNone);
        #ifdef USE_RC5
        signal.rise(&RC5::decode_rise);
        signal.fall(&RC5::decode_fall);
        #elifdef USE_SIRC
        signal.rise(&SIRC::decode_rise);
        signal.fall(&SIRC::decode_fall);
        #else
        signal.fall(&bootstrap);
        #endif
    }

    void debug() {
        #ifdef DEBUG_MODE
        if (start_bit1) {
            DEBUG::print("Fall\r\n");
            start_bit1 = false;
        }
        if (start_bit2) {
            DEBUG::print("Rise\r\n");
            start_bit2 = false;
        }
        
        if (decoded) {
            DEBUG::print("com %d\r\n", command);
            decoded = false;
        }

        if (bootstrapped) {
            DEBUG::print("bootstrapped\r\n");
        }
        #endif
    }
}
