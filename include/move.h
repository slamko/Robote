#ifndef MOVE_H
#define MOVE_H

#include "mbed.h"

namespace Move {
    enum Err {
        NUL = 0,
        MIN = 1, 
        MOYENNE = 2,
        IMPORTANTE = 3,
        MAX = 4,
        URGENTE = 5
    };

    int demi_tour_balise_gauche();
    int demi_tour_balise_droite();
    int demi_tour_croisement();
    int tour_balise_gauche();
    int tour_balise_droite();

    void control();

    void mise_en_marche();
    
    void stop();

    void init();
}
#endif