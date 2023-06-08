#include "mbed.h"
#include "include/config.h"
#include "include/move.h"
#include "include/lir.h"
#include "include/outils.h"
#include "include/debug.h"

namespace Challenge {
    const int DEMI_TOUR_WAIT_TIME = usec(150000us).count(); 
    const int SINGLE_BANDE_WAIT_TIME = usec(100000us).count();

    static Timer demi_tour_timer {};
    //static Timer single_bande_disable_timer {};

    unsigned int double_gauche_balise_counter;
    unsigned int double_droite_balise_counter;
    unsigned int croisement_counter;
    unsigned int balise_droite_counter;
    unsigned int balise_gauche_counter;

    bool double_gauche_balise = false;
    bool balise_gauche = false;
    bool double_droite_balise = false;
    bool balise_droite = false;
    bool waiting_cahallenge = false;
    bool challenge_active = false;
    bool en_croisement = false;

    size_t cur_challenge;

    struct Challenge {
        Callback<bool()> predicate;
        Callback<void()> reset;
        Callback<int()> action;
        usec delay;
    };

    static Challenge challenges[] {
        { 
            .predicate = [] () {
                return balise_droite_counter >= 1;
            },
            .reset = [] () {
                balise_droite_counter = 0;
            },
            .action = &Move::tour_balise_droite,
            .delay = 0us,
        },
        { 
            .predicate = [] () {
                return double_gauche_balise_counter >= 1;
            },
            .reset = [] () {
                double_gauche_balise_counter = 0;
            },
            .action = &Move::tour_balise_gauche,
            .delay = 150000us,
        },
        { 
            .predicate = [] () {
                return double_droite_balise_counter >= 1;
            },
            .reset = [] () {
                double_droite_balise_counter = 0;
            },
            .action = &Move::tour_balise_droite,
            .delay = 150000us,
        },
    };

    void balise_counter() {
        if (challenge_active) return;

        if (en_croisement && !LIR::croisement()) {
            en_croisement = false;
            croisement_counter ++;
        }

        if (!double_gauche_balise && LIR::balise_raccourci() && LIR::lg) {
            double_gauche_balise = true;
        }

        if (double_gauche_balise && !LIR::balise_raccourci() && !LIR::lg) {
            double_gauche_balise = false;
            double_gauche_balise_counter ++;
            balise_gauche_counter -= 2;
        }

        if (!double_droite_balise && LIR::balise_priorite() && LIR::ld) {
            double_droite_balise = true;
        }

        if (double_droite_balise && !LIR::balise_priorite() && !LIR::ld) {
            double_droite_balise = false;
            double_droite_balise_counter ++;
            balise_droite_counter -= 2;
        }
  
        if (!balise_droite && LIR::balise_priorite()) {
            balise_droite = true;
        }

        if (balise_droite && !LIR::balise_priorite()) {
            balise_droite = false;
            balise_droite_counter ++;
        }

        if (!balise_gauche && LIR::balise_raccourci()) {
            balise_gauche = true;
        }

        if (balise_gauche && !LIR::balise_raccourci()) {
            balise_gauche = false;
            balise_gauche_counter ++;
        }

        if (!en_croisement && LIR::croisement()) {
            en_croisement = true;
        }
    }

    static inline void demi_tour_start() {
        demi_tour_timer.reset();
        demi_tour_timer.start();
        challenge_active = true;    
        DEBUG::print("Demi tour start");
    }

    bool en_cours() {
        return challenge_active;
    }

    void control() {
        balise_counter();

        if (!challenge_active) {
            for (int i = 0; i < ARR_SIZE(challenges); i++) {
                if (challenges[i].predicate()) {
                    cur_challenge = i;
                    waiting_cahallenge = true;
                    demi_tour_start();
                    break;
                }
            }
        }

        if (demi_tour_timer.elapsed_time() > challenges[cur_challenge].delay) {
            if (challenges[cur_challenge].action()) {
                challenge_active = false;
                DEBUG::print("Challenge elapsed time %d\r\n", demi_tour_timer.elapsed_time().count());
                DEBUG::print("Demi tour fini");
                
                challenges[cur_challenge].reset();

                demi_tour_timer.stop();
                demi_tour_timer.reset();
            }
        }
    }
}