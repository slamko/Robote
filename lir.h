#ifndef LIR_H
#define LIR_H

#include "pinout.h"

namespace LIR {
    class lir {
    public:
        lir(PinName pin, bool inverse);
        template<class R>
        R read();
    private:
        bool inverse_read;
        DigitalIn capteur;
    };

    extern lir lir1;
    extern lir lir2;
    extern lir lir3;
    extern lir lir4;
    extern lir lir5;
    extern lir lir6;
    extern lir lir7;
    extern lir lir8;
}

#endif