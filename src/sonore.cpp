#include <InterruptIn.h>

#include "include/config.h"
#include "include/sonore.h"
#include "include/events.h"
#include "include/debug.h"
#include "include/outils.h"

using namespace std;
using namespace chrono;

namespace Sonore {
    const usec SONORE_RESTART_INT = 60000us; 
    const usec TRIG_PULSE_DUR = 10us; 
    const usecf SOUND_SPEED = 0.034us;
    const float OBSTACLE_DETECT_DIST = 40.0f;
    
    bool echo_rise_act {false};
    bool echo_fall_act {false};

    float distance = OBSTACLE_DETECT_DIST + 1.0f;
    usec echo_pulse_dur;

    DigitalOut trig {TRIG};
    InterruptIn echo {ECHO};
    Timer echo_timer;
    Timer trig_timer;
    Timer trig_repeat_timer;

    bool echo_rise = false;
    bool echo_fall = false;

    static void gen_trig_pulse() {
        trig = 1;
       // DEBUG::print("trig\n\r");
        trig_timer.reset();
        trig_timer.start();
    }

    static void check_trig_pulse() {
        if (!trig) return;

        if (trig_timer.elapsed_time() > TRIG_PULSE_DUR) {
            trig = 0;
            //DEBUG::print("fin_trig\n\r");
            trig_timer.stop();
        }
    }

    static void on_echo_rise() {
        echo_timer.reset();
        echo_timer.start();
        echo_rise = true;
        //DEBUG::nb_print("echo rise\r\n");
    }

    static void on_echo_fall() {
        echo_timer.stop();
        echo_pulse_dur = echo_timer.elapsed_time();
        distance = get_obstacle_dist();
        echo_fall = true;
       // DEBUG::nb_print("echo fall\r\n");
    }

    float get_obstacle_dist() {
        return (SOUND_SPEED.count() * echo_pulse_dur.count()) / 2.0f;
    }
    
    void control() {
        check_trig_pulse();
        if (echo_rise) {
            echo_rise = false;
        }

        if (trig_repeat_timer.elapsed_time() > SONORE_RESTART_INT) {
            trig_repeat_timer.reset();
            gen_trig_pulse();
        }
    }

    void debug() {
    #ifdef DEBUG_SONORE
        control();
        DEBUG::print("echo dist: %d \r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));

        if (echo_rise) {
            DEBUG::print("echo rise\r\n");
            echo_rise = false;
        } else if (echo_fall) {
            DEBUG::print("echo false\r\n");
            echo_fall = false;
        }
    #endif
    }

    bool obstacle_detected() {
        return distance < OBSTACLE_DETECT_DIST;
    }

    void run_sonore() {
        gen_trig_pulse();
    }

    void start() {
        trig_repeat_timer.reset();
        trig_repeat_timer.start();

        echo.rise(&on_echo_rise);
        echo.fall(&on_echo_fall);

        run_sonore();
    }

    void stop() {
        trig_repeat_timer.stop();
        trig_repeat_timer.reset();

        echo.rise(NULL);
        echo.fall(NULL);
    }

    void init() {
        echo.mode(PullDown);
    }
}