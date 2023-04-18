#include "mbed.h"
#include <InterruptIn.h>
#include "include/config.h"
#include "include/outils.h"
#include "include/move.h"
#include "include/lir.h"
#include "include/pid.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/events.h"

namespace Move {
    #ifdef DEBUG_MODE
    const usec PID_Sample_Rate = 500000us; 
    #else
    const usec PID_Sample_Rate = 1000us; 
    #endif

    static Timer pid_timer;
    static InterruptIn arrivee_in{ARRIVEE, PullUp};

    bool priorite;
    bool arrivee;
    bool arrivee_decl;
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
    static void check_priorite() {
        if (priorite) {
            if (Sonore::obstacle_detected()) {
                arret = true;

                H::arret();
            } else {
                priorite = false;
            }
        }
    }

    static void arrivee_control() {
        if (!arrive) return;

        if (LIR::un()) {
            arrivee = false;
        }
    }

    void control() {
        if (!Outil::at_time(pid_timer, PID_Sample_Rate)) return;

        LIR::read();

        const int 
            l1 = LIR::lir1, l2 = LIR::lir2, 
            l3 = LIR::lir3, l4 = LIR::lir4, 
            l5 = LIR::lir5, l6 = LIR::lir6,
            l7 = LIR::lir7, l8 = LIR::lir8;
        
        arrivee_control();

        if (!arrivee) {
            PID::calcul();
        }
    }

    static void init_arrivee_int() {
        arrivee_in.mode(PullUp);
        arrivee_in.fall([]() {
            arrivee = true;
            Events::call(&H::arret());
        });
    }

    void init() {
        pid_timer.start();
        PID::init();
    }
}