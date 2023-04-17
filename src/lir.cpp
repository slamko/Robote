#include "mbed.h"
#include "include/config.h"
#include "include/lir.h"

namespace LIR {

    static bool inverse { piste };

    bool inverse_read() {
        return inverse;
    }

    void init(bool inv) {
        inverse = inv;
    }

    template <class T>
    lir<T>::lir(PinName pin) : capteur(pin) {}

    template <class Capteur, class Data>
    input<Capteur, Data>::input(PinName pin) : capteur(pin) {}

    Analog::Analog(PinName pin) : input(pin) {}

    void Analog::read() {
        if (LIR::inverse_read()) {
            data = (1.0f - capteur.read());
        } else {
            data = capteur.read();
        }
    }

    Digital::Digital(PinName pin) : input(pin) {}

    void Digital::read() {
        if (LIR::inverse_read()) {
            data = !capteur.read();
        } else {
            data = capteur.read();
        }
    }
/*
    template <class Capteur, class Ret>
    lirArray<Capteur, Ret>::lirArray(Capteur lir1, Capteur lir2, Capteur lir3, Capteur lir4,
                Capteur lir5, Capteur lir6, Capteur lir7, Capteur lir8)
            : lir1(lir1), lir2(lir2), lir3(lir3), lir4(lir4),  
                lir5(lir5), lir6(lir6), lir7(lir7), lir8(lir8)
            {}

    void lirArray::read() {
            lir1.read();
            lir2.read();
            lir3.read();
            lir4.read();
            lir5.read();
            lir6.read();
            lir7.read();
            lir8.read();
        }
*/

    void lirArray::read() {
        for(auto capteur : lir) {
            capteur->read();
        }
    }

    static Digital lir1{LIR1};
    static Digital lir2{LIR2};
    static Digital lir3{LIR3};
    static Digital lir4{LIR4};
    static Digital lir5{LIR5};
    static Digital lir6{LIR6};
    static Digital lir7{LIR7};
    static Digital lir8{LIR8};

    input_read *lirArray::lir[8] = {
        &lir1,
        &lir2,
        &lir3,
        &lir4,
        &lir5,
        &lir6,
        &lir7,
        &lir8
    };

    lirArray array = lirArray();
}