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
    const usec PID_Sample_Rate = 4600us; 
    #endif

    const usec RACOURCI_TIME = 150000us;
    const int MIS_EN_MARCHE_TIME = usec(5000us).count(); 

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
    bool ldroit_on = false;
    bool lgauche_on = false;
    bool arrivee = false;
    bool balise_gauche = false;
    bool balise_droite = false;
    bool en_croisement = false;
    bool racourci_prevoir = false;
    bool racourci_gauche = false;
    bool racourci_pris = false;
    bool fin_racourci = false;
    bool rotation_360 = false;
    bool racourci = false;
    bool en_raccourci = false;
    bool rotation_fini = false;

    unsigned int croisement_counter;
    unsigned int balise_droite_counter;
    unsigned int balise_gauche_counter;

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

        if (rotation_360) {
            return Err::URGENTE;
        }

        if (fin_racourci && racourci_gauche) {
            return Err::URGENTE;
        } else if (fin_racourci) {
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

/*
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
*/

    static void figure_control() {
        using LIR::l1;
        using LIR::ld;

        if (ld && l1) {
            rotation_360 = true;
        }
    }

    static void demi_tour() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
        }

        if (l8) {
            rotation_fini = true;
            rotation_360 = false;
        }
    }

    static void priorite_control() {
        Sonore::control();
        if (priorite) {
            DEBUG::print("echo dist: %d \r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));

            if (!arret && Sonore::obstacle_detected()) {
                DEBUG::print("obstacle\r\n");
                stop();
            }
            
            if (LIR::croisement()) {
                priorite = false;
                PID::set_max_out(MAX_VITESSE);
                DEBUG::print("croisement\r\n");
                //Sonore::stop();
            }

            if (arret && !Sonore::obstacle_detected()) {
                DEBUG::print("mise en marche\r\n");
                
                priorite = false;
                mise_en_marche();
                //Sonore::stop();
                PID::set_max_out(MAX_VITESSE);
                wait_us(MIS_EN_MARCHE_TIME);
            }
            
        } else  if (LIR::balise_priorite() && !priorite) {
            priorite = true;
            DEBUG::print("Priorite\r\n");
        }
        else if (Sonore::obstacle_proche()) {
            DEBUG::print("obstacle proche\r\n");
            PID::set_max_out(FREIN_VITESSE);
        } else {
            PID::set_max_out(MAX_VITESSE);
        }
    }

    void balise_counter() {
        if (!balise_droite && LIR::balise_priorite()) {
            balise_droite = true;
        }

        if (balise_droite && !LIR::balise_priorite()) {
            balise_droite = false;
            balise_droite_counter ++;
        }

        if (!en_croisement && LIR::croisement()) {
            en_croisement = true;
        }

        if (en_croisement && !LIR::croisement()) {
            en_croisement = false;
            croisement_counter ++;
        }


    }

    static void racourci_control() {
        using LIR::l1;
        using LIR::l8;

        if (LIR::balise_raccourci() 
                &&!balise_gauche && !racourci_prevoir 
                && !racourci && !racourci_pris && !fin_racourci) {

            balise_gauche = true;
            DEBUG::print("balise gauche\r\n");
        }

        if (balise_gauche && !l8) {
            DEBUG::print("racourci prevoir\r\n");
            balise_gauche = false;
            racourci_prevoir = true;
            balise_gauche_counter ++;
        }
        
/*
        if (racourci_prevoir && (error >= 3)) {
            racourci_prevoir = false;
            DEBUG::print("cancel racourci\r\n");
        }
*/     
 
        if (racourci_prevoir) {
            if (LIR::croisement() || (LIR::piste_gauche() && LIR::piste_droite())) {
                racourci_prevoir = false;
            }

            if (LIR::piste_gauche()) {
                racourci = true;
                racourci_gauche = true;
            }  
            else if (LIR::piste_droite()) {
                racourci = true;
                racourci_gauche = false;
            }

            if (racourci) {
                DEBUG::print("racourci\r\n");
                racourci_prevoir = false;
                balise_gauche = false;
            }
        } 

        if (racourci) {
            if (!ldroit_on && l1) {
                ldroit_on = true;
                DEBUG::print("Ldroit active\r\n");
            }
            if (ldroit_on && !l1) {
                racourci = false;
                ldroit_on = false;
                racourci_gauche = false;
                balise_gauche = false;
                racourci_pris = true;
                DEBUG::print("Racourci pris\r\n");
            }

            DEBUG::print("Racourci\r\n");
        }

        if (racourci_pris) {
            DEBUG::print("Racourci en train\r\n");
            if (fin_racourci) {

#ifdef RACCOURCI_GAUCHE
                if (lgauche_on && !l8)
#else
                if (ldroit_on && !l1)
#endif
                {
                    DEBUG::print("tourne\r\n");
                    fin_racourci = false;
                    racourci = false;
                    racourci_prevoir = false;
                    racourci_pris = false;
                    lgauche_on = false;
                    ldroit_on = false;
                    racourci_gauche = false;
                }
            }

            if (LIR::croisement() && !fin_racourci) {
                DEBUG::print("Raccourci croisement\r\n");
#ifdef RACCOURCI_GAUCHE

                if (l8) {
                    lgauche_on = true;
                    racourci_gauche = true;
                    DEBUG::print("croise: lgauche\r\n");
                }    
#else

                if (l1) {
                    ldroit_on = true;
                    racourci_gauche = false;
                    DEBUG::print("croise: ldroit\r\n");
                }     
#endif

                fin_racourci = true;
            }
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

#ifdef PRIORITE_ENABLE
        if (!racourci && !fin_racourci) {
            priorite_control();
        }
#endif

        if (!arret) {
            error = pid_error();

#ifdef RACCOURCI_ENABLE
            racourci_control();
#endif

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
        Sonore::start();
        init_arrivee_timer();
        mise_en_marche();
    }
}