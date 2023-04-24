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
#include "include/debug.h"

namespace Move {
    #ifdef DEBUG_MODE
    const usec PID_Sample_Rate = 500000us; 
    #else
    const usec PID_Sample_Rate = 1000us; 
    #endif

    static Timer pid_timer;
    static InterruptIn arrivee_in {ARRIVEE, PullUp};

    static int8_t prev_error;
    static int8_t cur_error;

    bool priorite;
    bool arrivee;
    bool arret;
    bool balise_gauche;
    bool racourci_prevoir;
    bool racourci_gauche;
    bool racourci_droit;

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

    static int8_t pid_error() {
        using namespace LIR;

        DEBUG::fprint<int, int, int, int, int, int, int, int>(l1, l2, l3, l4, l5, l6, l7, l8);
        
        if (LIR::nul())  {
            if (prev_error > 0) {
                return 5;
            }
            if (prev_error < 0) {
                return -5;
            }
        }
        
        DEBUG::fprint<int, int, int, int, int, int, int, int>(l1, l2, l3, l4, l5, l6, l7, l8); 
        
        if (l4 &&  l5) return 0;

        if (l6 && !l7) return 1;
        if (l3 && !l2) return -1;
        if (l6 && l7 && !l8) return 2;
        if (l3 && l2) return -2;
        if (l7 && l8) return 3;
        if (l2 && l1) return -3;
        if (!l7 && l8) return 4;
        if (!l2 && l1) return -4;

        return 0;
    }

    static void priorite_control() {
        using namespace LIR;

        if (l1 && (l6 || l5 || l4 || l3) && !l2 && !( l8 || l7)) {
            priorite = true;
            Sonore::start();
        }

        if (priorite) {
            Sonore::control();
            DEBUG::fprint("echo dist: ", (int)(Sonore::get_obstacle_dist() * 1.0f));
            
            if (!arret && Sonore::obstacle_detected()) {
                arret = true;
                H::arret();
            }

            if (croisement()) {
                priorite = false;
                Sonore::stop();
            }

            if (arret && !Sonore::obstacle_detected()) {
                arret = false;
                H::mise_en_marche();
            }
        }
    }

    static void racourci_control() {
        using namespace LIR;

        if (l8 && (l6 || l5 || l4 || l3) && !(l1 || l2)) {
            balise_gauche = true;
        }

        if (balise_gauche && !l8) {
            racourci_prevoir = true;
            balise_gauche = false;
        }

        if (racourci_prevoir) {
            if (l8 && l7 && (l6 || l5 || l4 || l3) && !(l1 || l2)) {
                racourci_gauche = true;
                racourci_prevoir = false;
            } else if (!(l8 || l7) && (l6 || l5 || l4 || l3) && l2 && l1) {
                racourci_droit = true;
                racourci_prevoir = false;
            }
        } 
        
        if (racourci_gauche) {
            cur_error = 5;
        } else if (racourci_droit) {
            cur_error = -5;
        }
    }

    static void arrivee_control() {
        if (!arrivee) return;

        if (LIR::un()) {
            arrivee = false;
        }

        if (arrivee && !arret) {
            H::arret();
            arret = true;
        }
    }

    void control() {
        if (pid_timer.elapsed_time() < PID_Sample_Rate) return;

        LIR::read();
        arrivee_control();

        if (!arret) {
            cur_error = pid_error();
            
            racourci_control();
            priorite_control();
            PID::calcul(cur_error, prev_error);

            prev_error = cur_error;
        }
    }

    void init() {
        pid_timer.start();
        PID::init();
        
        arrivee_in.mode(PullUp);
        arrivee_in.fall([]() {
            if (LIR::nul()) {
                arrivee = true;
            }
        });
    }
}