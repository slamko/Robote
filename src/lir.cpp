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

    template <class Capteur, class Data>
    input<Capteur, Data>::input(PinName pin) : capteur(pin), data{} {}

    void Analog::read() {
        if (LIR::inverse_read()) {
            data = (1.0f - capteur.read());
        } else {
            data = capteur.read();
        }
    }

    void Digital::read() {
        if (LIR::inverse_read()) {
            data = !capteur.read();
        } else {
            data = capteur.read();
        }
    }

    void lirArray::read() {
        for(auto capteur : lir) {
            capteur->read();
        }
    }

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

    Digital lir1{LIR1};
    Digital lir2{LIR2};
    Digital lir3{LIR3};
    Digital lir4{LIR4};
    Digital lir5{LIR5};
    Digital lir6{LIR6};
    Digital lir7{LIR7};
    Digital lir8{LIR8};
}