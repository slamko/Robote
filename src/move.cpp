#include "include/move.h"

#include "mbed.h"
#include "include/config.h"
#include "include/outils.h"
#include "include/move.h"
#include "include/lir.h"
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
    bool arret;

/*
    static void verif_arrivee() {
        const int 
            l1 = LIR::lir1, l2 = LIR::lir2, 
            l3 = LIR::lir3, l4 = LIR::lir4, 
            l5 = LIR::lir5, l6 = LIR::lir6,
            l7 = LIR::lir7, l8 = LIR::lir8;

        if ((l6 + l7 + l8 + l5 + l3 + l2 + l1 + l4) >= 6)  {
            arrivee = true;
        }

        if ((!l6 && !l7 && !l8 && !l5 && !l3 && !l2 && !l1 && !l4) && arrivee)  {
            
        }

        if(arrivee) {
            H::arreter();
        }
    }
*/

    void control() {
        LIR::array.read();

        const int 
            l1 = LIR::lir1, l2 = LIR::lir2, 
            l3 = LIR::lir3, l4 = LIR::lir4, 
            l5 = LIR::lir5, l6 = LIR::lir6,
            l7 = LIR::lir7, l8 = LIR::lir8;

        if (Outil::at_time(pid_timer, PID_Sample_Rate)) return;

        if (priorite) {
            if (Sonore::obstacle_detected()) {
                arret = true;

                H::arreter();
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