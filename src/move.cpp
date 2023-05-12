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
    const usec PID_Sample_Rate = 800us; 
    #endif

    const usec RACOURCI_TIME = 150000us;

    static Timer pid_timer {};
    static Timer racourci_timer {};
    static Timer arret_timer {};
#ifndef DEBUG_MODE
    static InterruptIn arrivee_in {ARRIVEE};
#endif

    using error_t = int8_t;
    static error_t prev_error;
    static error_t error;

    bool arret = true;
    bool priorite = false;
    bool l1_on;
    bool l1_off = true;
    bool arrivee = false;
    bool balise_gauche = false;
    bool racourci_prevoir = false;
    bool racourci_gauche = false;
    bool priorite_arret = false;
    bool rotation_360 = false;
    bool racourci = false;

    void mise_en_marche() {
        if (!arret) return;

        pid_timer.reset();
        pid_timer.start();

        arret = false;
        H::mise_en_marche();
    }

    void stop() {
        if (arret) return;

        arret = true;
        H::arret();
    }

    static error_t pid_error() {
        using namespace LIR;

        DEBUG::print("%d %d %d %d %d %d %d %d\r\n", (int)l1, (int)l2, (int)l3, (int)l4, (int)l5, (int)l6, (int)l7, (int)l8);

        if (racourci && racourci_gauche) {
            return Err::URGENTE;
        } else if (racourci) {
            return -Err::URGENTE;
        }
        
        if (LIR::nul()) {
            if (prev_error > 0) {
                return Err::URGENTE;
            }
            if (prev_error < 0) {
                return -Err::URGENTE;
            }
        }
        
       // DEBUG::print("%d %d %d %d %d %d %d %d\r\n", (int)l1, (int)l2, (int)l3, (int)l4, (int)l5, (int)l6, (int)l7, (int)l8); 
        
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

    static void verif_arrivee() {
        using namespace LIR;

        if ((l1 + l2 + l3 + l4 + l5 + l6 + l7 + l8) >= 4)  {
            arrivee = true;
        } else if (arrivee && LIR::un()) {
            arrivee = false;
        }

        if (LIR::nul() && arrivee)  {
            stop();
        }
    }

    static void figure_control() {
        using namespace LIR;

        if (ld && l1) {
            rotation_360 = true;
        }
    }

    static bool balise_priorite() {
        using namespace LIR;
        return (l1 && l5 && l4 && !( l8 || l7));
    }

    static bool balise_racourci() {
        using namespace LIR;
        return (l8 && (l6 || l5 || l4 || l3) && !(l1 || l2));
    }

    static void priorite_control() {
        using namespace LIR;

        if (priorite) {
            Sonore::control();
            DEBUG::print("echo dist: %d \r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));
           /* 
            if (priorite_arret && balise_priorite()) {
                H::arret_motors();
                arret = true;
            }
*/
            if (!arret && Sonore::obstacle_detected()) {
                DEBUG::print("obstacle\r\n");
                stop();
            }
            

            if (croisement()) {
                priorite = false;
                DEBUG::print("croisement\r\n");
                Sonore::stop();
            }


            if (arret && !Sonore::obstacle_detected()) {
                priorite = false;
                mise_en_marche();
                Sonore::stop();
                DEBUG::print("mise en marche\r\n");
                wait_us(5000);
            }
            
        } else  if (balise_priorite()) {
            priorite = true;
            Sonore::start();
            DEBUG::print("Priorite\r\n");
        }
    }

    static void racourci_control() {
        using namespace LIR;

        if (!balise_gauche && balise_racourci()) {
            balise_gauche = true;
           // racourci_prevoir = true;
            //wait_us(300000);
            DEBUG::print("balise gauche\r\n");
        }

        if (balise_gauche && !l8) {
            DEBUG::print("racourci prevoir\r\n");
            balise_gauche = false;
            racourci_prevoir = true;
           /* PID_Max_Out = 0.25f;
            PID_Min_Out = -0.25f;*/
        }
        
/*
        if (racourci_prevoir && (error >= 3)) {
            racourci_prevoir = false;
            DEBUG::print("cancel racourci\r\n");
        }
*/      
        if (racourci_prevoir) {
            if (l8 && l7 && (l6 || l5 || l4 || l3) && !(l1 || l2)) {
                racourci = true;
                racourci_gauche = true;
            } 
            /*
            else if (!(l8 || l7) && (l6 || l5 || l4 || l3) && l2 && l1) {
                racourci = true;
                racourci_gauche = false;
            }
*/
            if (racourci) {
                DEBUG::print("racourci\r\n");
                racourci_prevoir = false;
                //racourci = false;
                PID_Max_Out = 0.9f;
                PID_Min_Out = -0.9f;
                /*
                racourci_timer.reset();
                racourci_timer.start();
                PID_Max_Out = 0.9f;
                PID_Min_Out = -0.9f;
                */
            }
        } 

        if (racourci) {
            if (l1_off && l1) {
                l1_off = false;
            }
            if (!l1_off && !l1) {
                racourci = false;
                racourci_gauche = false;
                racourci_prevoir = false;
            }
            /*
            if (racourci_timer.elapsed_time() > RACOURCI_TIME) {   
                racourci_timer.stop();
                
                
            }
            */
        }
    }

    static void arrivee_control() {
        if (!arrivee) return;

        if (!arret) {
            arret = true;
            pid_timer.stop();
            H::arrivee();
        }
    }

    void control() {
        if (pid_timer.elapsed_time() < PID_Sample_Rate) return;

        LIR::read();
        arrivee_control();
        Sonore::debug();

        priorite_control();

        if (!arret && !priorite_arret) {
            error = pid_error();
            racourci_control();

            PID::calcul(error, prev_error);
            prev_error = error;
        }

        pid_timer.reset();
    }

    static void init_arrivee_timer() {
        #ifndef DEBUG_MODE

        arrivee_in.mode(PullNone);
        arrivee_in.rise([]() {
            arrivee = true;
        });
        #endif
    }

    void init() {
       // Sonore::start();
        //racourci_prevoir = true;
        init_arrivee_timer();
        mise_en_marche();
    }
}