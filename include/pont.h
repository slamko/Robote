#ifndef PONT_H
#define PONT_H

#include "mbed.h"

namespace H {

    float moteur_droit(float val);

    float moteur_gauche(float val);

    void moteur_droit_arriere();

    void moteur_droit_avant();

    void moteur_gauche_arriere();

    void moteur_gauche_avant();

    void arret();

    void init();

}

#endif