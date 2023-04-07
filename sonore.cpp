/*#include "mbed.h"

#include "pinout.h"

#include <InterruptIn.h>
#include <chrono>
#include <cstdint>
#include <ratio>


using namespace std;
using namespace chrono;


// Capteur Sonore
const usec SONORE_RESTART_INT = 60000us; 
const usec TRIG_PULSE_DUR = 10us; 
const usecf SOUND_SPEED = 0.034us;

//class Sonore {


    DigitalOut trig(TRIG);
    InterruptIn echo(ECHO);
    usec echo_pulse_dur;
    Timer echo_timer;
    Timer trig_timer;
    Timer trig_repeat_timer;


    static void gen_trig_pulse() {
        trig = 1;
        //printf("trig\n\r");
        trig_timer.start();
    }

    static void check_trig_pulse() {
        if (!trig) return;

        if (trig_timer.elapsed_time() >= TRIG_PULSE_DUR) {
            trig = 0;
            //printf("fin_trig\n\r");
            trig_timer.stop();
        }
    }

    static void sonore_ctrl() {
        check_trig_pulse();

        if (trig_repeat_timer.elapsed_time() > SONORE_RESTART_INT) {
            trig_repeat_timer.reset();
            gen_trig_pulse();
        }
    }

    static void run_sonore() {
        gen_trig_pulse();
    }

    static void on_echo_rise() {
        echo_timer.start();
    // printf("rise\n\r");
    // led = 1;
    }

    static void on_echo_fall() {
        echo_timer.stop();
        //printf("fall\n\r");
        //led = 0;
        echo_pulse_dur = echo_timer.elapsed_time();
    }

    static float get_obstacle_dist(usec pulse_dur) {
        return (SOUND_SPEED.count() * pulse_dur.count()) / 2.0f;
    }

    void sonore_init() {

        echo.rise(&on_echo_rise);
        echo.fall(&on_echo_fall);
        echo.mode(PullDown);
    }
//};

*/