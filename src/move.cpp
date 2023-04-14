#include "include/move.h"

#include "mbed.h"
#include "include/config.h"
#include "include/move.h"
#include "include/pid.h"
#include "include/sonore.h"
#include "include/pont.h"

namespace Move {
    #ifdef DEBUG_MODE
    const usec PID_Sample_Rate = 500000us; 
    #else
    const usec PID_Sample_Rate = 1000us; 
    #endif

    Timer pid_timer;
    bool priorite;
    bool arrivee;

    void control() {
        if (pid_timer.elapsed_time().count() < PID_Sample_Rate) return;

        if (priorite) {
            if (Sonore::get_obstacle_dist() < 50.0f) {
                arrivee = true;

                H::arrivee();
            } else {
                priorite = false;
            }
        }

        PID::calcul();
    }

    void init() {
        pid_timer.start();
        PID::init();
    }
}