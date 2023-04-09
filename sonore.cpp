
#include <InterruptIn.h>
#include <chrono>
#include <cstdint>
#include <ratio>

#include "pinout.h"
#include "sonore.h"


using namespace std;
using namespace chrono;

namespace Sonore {

    const usec SONORE_RESTART_INT = 60000us; 
    const usec TRIG_PULSE_DUR = 10us; 
    const usecf SOUND_SPEED = 0.034us;


    DigitalOut trig = DigitalOut(TRIG);
    InterruptIn echo(ECHO);
    usec echo_pulse_dur;
    Timer echo_timer;
    Timer trig_timer;
    Timer trig_repeat_timer;


    void gen_trig_pulse() {
        trig = 1;
        //printf("trig\n\r");
        trig_timer.start();
    }

    void check_trig_pulse() {
        if (!trig) return;

        if (trig_timer.elapsed_time() >= TRIG_PULSE_DUR) {
            trig = 0;
            //printf("fin_trig\n\r");
            trig_timer.stop();
        }
    }

    void on_echo_rise() {
        echo_timer.start();
    // printf("rise\n\r");
    // led = 1;
    }

    void on_echo_fall() {
        echo_timer.stop();
        //printf("fall\n\r");
        //led = 0;
        echo_pulse_dur = echo_timer.elapsed_time();
    }
    

    void sonore_ctrl() {
        check_trig_pulse();

        if (trig_repeat_timer.elapsed_time() > SONORE_RESTART_INT) {
            trig_repeat_timer.reset();
            gen_trig_pulse();
        }
    }

    void run_sonore() {
        gen_trig_pulse();
    }

    float get_obstacle_dist(usec pulse_dur) {
        return (SOUND_SPEED.count() * pulse_dur.count()) / 2.0f;
    }

    void sonore_init() {
        trig_repeat_timer.start();
    
        echo.rise(&on_echo_rise);
        echo.fall(&on_echo_fall);
        echo.mode(PullDown);
    }

}