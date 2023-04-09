#ifndef LIR_H
#define LIR_H

#include "pinout.h"

namespace LIR {
    template <class T>
    class lir {
    public:
        lir(PinName pin, bool inverse);
        
        template<class Ret>
        Ret read();

        template<class Ret>
        operator Ret();
    private:
        bool inverse_read;
        T capteur;
    };

    extern lir<DigitalIn> lir1;
    extern lir<DigitalIn> lir2;
    extern lir<DigitalIn> lir3;
    extern lir<DigitalIn> lir4;
    extern lir<DigitalIn> lir5;
    extern lir<DigitalIn> lir6;
    extern lir<DigitalIn> lir7;
    extern lir<DigitalIn> lir8;
}

#endif