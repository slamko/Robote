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

    void control();

    void init();
}
#endif