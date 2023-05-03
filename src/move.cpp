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

    const usec RACOURCI_TIME = 30000us;

    static Timer pid_timer {};
    static Timer racourci_timer {};
    static InterruptIn arrivee_in {ARRIVEE, PullUp};

    using error_t = int8_t;
    static error_t prev_error;

    bool arret = true;
    bool priorite = false;
    bool arrivee = false;
    bool balise_gauche = false;
    bool racourci_prevoir = false;
    bool racourci_gauche = false;
    bool rotation_360 = false;
    bool racourci = false;

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

    void mise_en_marche() {
        arret = false;
        H::mise_en_marche();
    }

    static error_t pid_error() {
        using namespace LIR;

        DEBUG::print("%d %d %d %d %d %d %d %d\r\n", (int)l1, (int)l2, (int)l3, (int)l4, (int)l5, (int)l6, (int)l7, (int)l8);

        if (racourci && racourci_gauche) {
            return Err::URGENTE;
        } else if (racourci) {
            return -Err::URGENTE;
        }

        if (rotation_360) {
            return Err::URGENTE;
        }
        
        if (LIR::nul())  {
            if (prev_error > 0) {
                return Err::URGENTE;
            }
            if (prev_error < 0) {
                return -Err::URGENTE;
            }
        }
        
        DEBUG::print("%d %d %d %d %d %d %d %d\r\n", (int)l1, (int)l2, (int)l3, (int)l4, (int)l5, (int)l6, (int)l7, (int)l8); 
        
        if (l4 &&  l5) return Err::NUL;

        if (l6 && !l7) return MIN;
        if (l3 && !l2) return -MIN;
        if (l6 && l7 && !l8) return MOYENNE;
        if (l3 && l2) return -MOYENNE;
        if (l7 && l8) return IMPORTANTE;
        if (l2 && l1) return -IMPORTANTE;
        if (!l7 && l8) return MAX;
        if (!l2 && l1) return -MAX;

        return 0;
    }

    static void figure_control() {
        using namespace LIR;

        if (ld && l1) {
            rotation_360 = true;
        }
    }

    static void priorite_control() {
        using namespace LIR;

        if (l1 && (l6 || l5 || l4 || l3) && !l2 && !( l8 || l7)) {
            priorite = true;
            Sonore::start();
        }

        if (priorite) {
            Sonore::control();
            DEBUG::print("echo dist: %d \r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));
            
            if (!arret && Sonore::obstacle_detected()) {
                arret = true;
                H::arret();
            }

            if (croisement()) {
                priorite = false;
                Sonore::stop();
            }

            if (arret && !Sonore::obstacle_detected()) {
                mise_en_marche();
            }
        }
    }

    static void racourci_control(const int error) {
        using namespace LIR;

        if (l8 && (l6 || l5 || l4 || l3) && !(l1 || l2)) {
            balise_gauche = true;
        }

        if (balise_gauche && !l8) {
            racourci_prevoir = true;
            balise_gauche = false;
        }

        if (racourci_prevoir && error) {
            racourci_prevoir = false;
        }

        if (racourci_prevoir) {
            if (l8 && l7 && (l6 || l5 || l4 || l3) && !(l1 || l2)) {
                racourci = true;
                racourci_gauche = true;
            } 
            else if (!(l8 || l7) && (l6 || l5 || l4 || l3) && l2 && l1) {
                racourci = true;
                racourci_gauche = false;
            }

            if (racourci) {
                racourci_prevoir = false;
                racourci_timer.reset();
                racourci_timer.start();
            }
        } 

        if (racourci) {
            if (racourci_timer.elapsed_time() > RACOURCI_TIME) {   
                racourci_timer.stop();
                racourci = false;
                racourci_gauche = false;
            }
        }
    }

    static void arrivee_control() {
        if (!arrivee) return;

        if (LIR::un()) {
            arrivee = false;
        }

        if (!arret) {
            H::arret();
            arret = true;
        }
    }

    void control() {
        if (pid_timer.elapsed_time() < PID_Sample_Rate) return;

        LIR::read();
        arrivee_control();

        if (!arret) {
            error_t error = pid_error();

            //priorite_control();
            //racourci_control(error);

            PID::calcul(error, prev_error);
            prev_error = error;
        }
    }

    static void init_arrivee_timer() {
        arrivee_in.mode(PullUp);
        arrivee_in.fall([]() {
            if (LIR::nul()) {
                arrivee = true;
            }
        });
    }

    void init() {
        pid_timer.start();
        PID::init();

        // init_arrivee_timer();
        mise_en_marche();
    }
}