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
#include "include/challenge.h"

namespace Move {
    #ifdef DEBUG_MODE
    const usec PID_Sample_Rate = 500000us; 
    #else
    const usec PID_Sample_Rate = 3400us; 
    #endif

    const usec RACOURCI_TIME = 150000us;
    const int MIS_EN_MARCHE_TIME = usec(30000us).count(); 
    const int RACCOURCI_PREVOIR_TIME = usec(500000us).count(); 
    const unsigned int INTERRUPT_AWAIT_TIME = usec(500000us).count();

    static Timer pid_timer {};
    static Timer racourci_timer {};
    static Timer arret_timer {};
    static Timer interrupt_timer {};
    static Timer arrivee_timer {};
    static Timer raccourci_prevoir_timer {};
#ifndef DEBUG_MODE
    static InterruptIn arrivee_in {ARRIVEE};
#endif

    using error_t = int8_t;
    static error_t prev_error;

    bool arret = true;
    bool priorite = false;
    bool ldroit_on = false;
    bool lgauche_on = false;
    bool arrivee = false;
    bool balise_gauche_raccourci = false;

    bool racourci_prevoir = false;
    bool racourci_gauche = false;
    bool racourci_pris = false;
    bool fin_racourci = false;
    bool rotation_360 = false;
    bool racourci = false;
    bool accelere = false;
    bool en_raccourci = false;
    bool demi_tour_l8_un = false;
    bool demi_tour_l8_nul = false;
    bool demi_tour_l8_deux = false;
    bool demi_tour_gauche = false;
    
    bool tour_complet_demi = false;
    bool tour_complet = false;
    bool premier = true;

    unsigned int no_echo_diff;
    unsigned int last_pid_timer = 0;
    unsigned int no_echo_last;
    
    long pid_timer_diff = INTERRUPT_AWAIT_TIME + 1;

    void mise_en_marche() {
        if (!arret) return;

        pid_timer.reset();
        pid_timer.start();
        Sonore::start();

        DEBUG::print("Mise en marche");
        arrivee = false;
        arret = false;
        
        if (premier) {
            premier = false;
        }

        interrupt_timer.reset();
        interrupt_timer.start();
            
        arrivee_timer.stop();
        arrivee_timer.reset();
        H::marche();
    }

    void stop() {
        if (arret) return;

        arret = true;
        H::arret();
    }

    static error_t pid_error() {
        using namespace LIR;

        DEBUG::fprint<int, int, int, int, int, int, int, int>(l1, l2, l3, l4, l5, l6, l7, l8);
        DEBUG::fprint<int, int>(ld, lg);

        if (racourci && racourci_gauche) {
            return Err::URGENTE;
        } else if (racourci) {
            return -Err::URGENTE;
        }

        if (rotation_360 && demi_tour_gauche) {
            return Err::URGENTE;
        } else if (rotation_360) {
            return -Err::URGENTE;
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

    int tour_balise_gauche() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
        }

        if (l1 || l2 && !tour_complet_demi) {
            tour_complet_demi = true;
        }

        if (tour_complet_demi && !l1) {
            tour_complet = true;
        }

        if (tour_complet && l1) {
            rotation_360 = false;
            tour_complet = false;
            tour_complet_demi = false;
            return 1;
        }
        return 0;
    }

    int tour_balise_droite() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
            demi_tour_gauche = true;
        }

        if (l8 || l7 && !tour_complet_demi) {
            tour_complet_demi = true;
        }

        if (tour_complet_demi && !l8) {
            tour_complet = true;
        }

        if (tour_complet && l8) {
            rotation_360 = false;
            tour_complet = false;
            tour_complet_demi = false;
            demi_tour_gauche = false;
            return 1;
        }
        return 0;
    }

    int demi_tour() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
        }

        if (l1 || l2) {
            rotation_360 = false;
            return 1;
        }
        return 0;
    }

    int demi_tour_balise_gauche() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
        }

        if (l1 || l2) {
            rotation_360 = false;
            return 1;
        }
        return 0;
    }

    int demi_tour_balise_droite() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
            demi_tour_gauche = true;
        }

        if (l8 || l7) {
            rotation_360 = false;
            demi_tour_gauche = false;
            return 1;
        }
        return 0;
    }

    int demi_tour_croisement() {
        using namespace LIR;

        if (!rotation_360) {
            rotation_360 = true;
            demi_tour_gauche = true;
        }

        if (l8 || l7 && !demi_tour_l8_un) {
            demi_tour_l8_un = true;
        } if (demi_tour_l8_un && !l8 && !demi_tour_l8_nul) {
            demi_tour_l8_nul = true;
        } if (demi_tour_l8_nul && l8) {
            rotation_360 = false;
            demi_tour_l8_un = false;
            demi_tour_l8_nul = false;
            demi_tour_l8_deux = false;
            demi_tour_gauche = false;
            return 1;
        }
        return 0;
    }

    static void priorite_control() {
        Sonore::control();
        if (priorite) {
            DEBUG::print("echo dist: %d \r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));
/*
            if (arret && !Sonore::obstacle_detected() && !Sonore::obstacle_proche()) {
                DEBUG::print("mise en marche\r\n");
                
                priorite = false;
                mise_en_marche();
                //Sonore::stop();
                PID::set_max_out(MAX_VITESSE);
                wait_us(MIS_EN_MARCHE_TIME);
            }
*/
            if (Sonore::no_obstacle_count() - no_echo_last > 4) {
                DEBUG::print("mise en marche\r\n");
                
                priorite = false;
                mise_en_marche();
                //Sonore::stop();
                PID::set_max_out(MAX_VITESSE);
                wait_us(MIS_EN_MARCHE_TIME);
            }

            if (!arret && (Sonore::obstacle_detected() || Sonore::obstacle_proche())) {
                DEBUG::print("obstacle\r\n");
                no_echo_last = Sonore::no_obstacle_count();
                stop();
            }
            
            if (LIR::croisement()) {
                priorite = false;
                PID::set_max_out(MAX_VITESSE);
                DEBUG::print("croisement\r\n");
                //Sonore::stop();
            }        
        } else  if (LIR::balise_priorite() && !priorite) {
            priorite = true;
            DEBUG::print("Priorite\r\n");
            if (Sonore::obstacle_detected() || Sonore::obstacle_proche()) {
                DEBUG::print("obstacle\r\n");
                no_echo_last = Sonore::no_obstacle_count();
                stop();
            }
        }
        else if (Sonore::obstacle_proche()) {
            DEBUG::print("obstacle proche\r\n");
            PID::set_max_out(FREIN_VITESSE);
        } else {
            PID::set_max_out(MAX_VITESSE);
        }
    }

    static void racourci_control() {
        using LIR::l1;
        using LIR::l8;

        if (LIR::balise_raccourci() 
                &&!balise_gauche_raccourci && !racourci_prevoir 
                && !racourci && !racourci_pris && !fin_racourci) {

            balise_gauche_raccourci = true;
            DEBUG::print("balise gauche\r\n");
        }

        if (balise_gauche_raccourci && !l8) {
            DEBUG::print("racourci prevoir\r\n");
            balise_gauche_raccourci = false;
            racourci_prevoir = true;
            raccourci_prevoir_timer.reset();
            raccourci_prevoir_timer.start();
        }
        
        if (racourci_prevoir) {
            if (raccourci_prevoir_timer.elapsed_time().count() > RACCOURCI_PREVOIR_TIME) {
                racourci_prevoir = false;
                raccourci_prevoir_timer.stop();
                return;
            }

            if (LIR::croisement() || (LIR::piste_gauche() && LIR::piste_droite())) {
                racourci_prevoir = false;
                raccourci_prevoir_timer.stop();
                return;
            }

            if (LIR::balise_raccourci()) {
                racourci_prevoir = false;
                raccourci_prevoir_timer.stop();
                return;
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
                balise_gauche_raccourci = false;
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
                balise_gauche_raccourci = false;
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

    static void accelerate_control() {
        if (!accelere && LIR::balise_priorite() && !Sonore::obstacle_proche()) {
            PID::set_max_out(ACC_VITESSE);
            accelere = true;
        } else if (accelere && (LIR::balise_priorite() || LIR::balise_raccourci() || LIR::croisement())) {
            PID::set_max_out(MAX_VITESSE);
            accelere = false;
        }
    }

    static void arrivee_control() {
        if (!arrivee) return;

        if (!arret) {
            arret = true;
            DEBUG::print("Arrivee\r\n");
            pid_timer.reset();
            H::arrivee();
        }
    }

    void control() {
        if (pid_timer.elapsed_time() < PID_Sample_Rate) return;

        LIR::read();
        arrivee_control();
        Sonore::debug();

        if (!arret) {          
#ifdef RACCOURCI_ENABLE
            racourci_control();
#endif
        }

#ifdef PRIORITE_ENABLE
        if (!racourci && !fin_racourci && !Challenge::en_cours()) {
            priorite_control();
        }
#endif

        if (!arret) {
#ifdef ACCELERATE_ENABLE
            accelerate_control();
#endif

#ifdef CHALLENGE_ENABLE
            Challenge::control();
#endif

            error_t error = pid_error();
            PID::calcul(error, prev_error);
            prev_error = error;
        }

        pid_timer.reset();
    }

    static void interrupteur() {
        if (arret) {
            if (arrivee_timer.elapsed_time().count() < INTERRUPT_AWAIT_TIME && !premier) return;
            mise_en_marche();
            
            //last_pid_timer = pid_timer.elapsed_time().count();
        } else {
            if (interrupt_timer.elapsed_time().count() < INTERRUPT_AWAIT_TIME) return;
            interrupt_timer.stop();
            interrupt_timer.reset();

            arrivee_timer.reset();
            arrivee_timer.start();
            //last_pid_timer = pid_timer.elapsed_time().count();
            arrivee = true;
        }
    }

    static void init_arrivee_timer() {
#ifndef DEBUG_MODE
        arrivee_in.mode(PullNone);
        arrivee_in.rise(&interrupteur);
#endif
    }

    void init() {
        arret = true;
        init_arrivee_timer();

        #ifdef DEBUG_MODE
        mise_en_marche();
        #endif
    }
}