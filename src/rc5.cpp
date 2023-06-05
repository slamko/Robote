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
    static const int med_pulse = 1500;
    static const usec max_long_pulse = 3000us; 
    static const uint8_t play_code = 12;

    static constexpr int COMMAND_LEN = 14;

    bool good_startcode() {
        return ((command & (1 << 0)) && (command & (1 << 1)));
    }
/*
    uint8_t inverse_bits(uint8_t num) {
        for (int i = 0; i < 2; i++) {
            num ^= (num & (1 << i)) << (5 - i);
            num ^= (num & (1 << (5 - i))) << (i);
            num ^= (num & (1 << i)) << (5 - i);
        }
        return num;
    }
*/
    uint8_t inverse_bits(uint8_t num) {
        uint8_t res = 0;
        for (int i = 0; i < 3; i++) {
            res |= (((num & (1 << i)) >> i) << (5 - i));
            res |= (((num & (1 << (5 - i))) >> (5 - i)) << i);
        }
        return res;
    }

    bool is_playcode() {
        return (inverse_bits(((command >> 8) & 0x3F)) == play_code);
    }

    void decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (cur_bit >= COMMAND_LEN) {
            decoded = true;
            decoding = false;
        }

        clock.reset();
    }

    void decode_fall() {
        //if (decoded) return;
        if (!decoding || clock.elapsed_time() > max_long_pulse) {
            decode_reset();
            decode_bit(1);
            start_bit1 = true;
            return;    
        }

        if (!GET_BIT(command, cur_bit - 1) && clock.elapsed_time().count() < med_pulse) {
            clock_restart();
            return;
        }

        start_bit1 = true;
        decode_bit(1);
    }

    void decode_rise() {
        if (!decoding) return;

        if (cur_bit && GET_BIT(command, cur_bit - 1) && clock.elapsed_time().count() < med_pulse) {
            clock_restart();
            return;
        }

        start_bit2 = true;
        decode_bit(0);
    }
}

namespace SIRC {
    static constexpr int COMMAND_LEN = 12;
    static constexpr usec start_bit_pulse = 1800us;
    static constexpr usec high_pulse = 1000us;
    static constexpr usec low_pulse = 400us;

    uint8_t play_code = 21;

    bool good_startcode() {
        return command & (1 << 0);
    }

    bool is_playcode() {
        return (good_startcode() && ((command >> 1) & 0x7F) == play_code);
    }

    void decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (cur_bit >= COMMAND_LEN) {
            decoded = true;
        }

        clock.reset();
    }

    void decode_fall() {
        if (decoded) return;

        if (!decoding || clock.elapsed_time() > start_bit_pulse * 2) {
            decode_reset();
            start_bit1 = true;
            return;    
        }

        start_bit1 = true;
        clock_restart();
    }

    void decode_rise() {
        if (!decoding || decoded) return;

        start_bit2 = true;
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
        signal.rise(&RC5::decode_rise);
        signal.fall(&RC5::decode_fall);
        /*
        #ifdef USE_RC5
        signal.rise(&RC5::decode_rise);
        signal.fall(&RC5::decode_fall);
        #endif
        #ifdef USE_SIRC
        #endif
        #ifdef TELECOMMANDE_BOOTSTRAP
        signal.fall(&bootstrap);
        #endif
        */
    }

    void control() {
      /*  #ifdef DEBUG_MODE
        if (start_bit1) {
            DEBUG::print("Fall\r\n");
            DEBUG::print("bit %d\r\n", cur_bit);
            start_bit1 = false;
        }
        if (start_bit2) {
            DEBUG::print("Rise\r\n");
            DEBUG::print("bit %d\r\n", cur_bit);
            start_bit2 = false;
        }
        if (bootstrapped) {
            DEBUG::print("bootstrapped\r\n");
        }
        #endif
*/

        if (decoded) {
            DEBUG::print("com %d\r\n", RC5::inverse_bits((command >> 8) & 0x3F));
            decoded = false;
            decoding = false;
            cur_bit = 0;
            clock.stop();
            clock.reset();

            if (RC5::is_playcode()) {
                Move::mise_en_marche();
            }
        }
    }
}
