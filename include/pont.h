#ifndef PONT_H
#define PONT_H

#include "mbed.h"

namespace H {

    float moteur_droit(float val);

    float moteur_gauche(float val);

    void moteur_droit_arriere();
    bool droit_arriere();

    void moteur_droit_avant();

    void moteur_gauche_arriere();
    bool gauche_arriere();

    void moteur_gauche_avant();

    void mise_en_marche();

    void arret();

    void init();
}

#endif