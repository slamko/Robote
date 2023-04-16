#include "mbed.h"
#include "include/config.h"
#include "include/lir.h"

namespace LIR {
    template <class T>
    lir<T>::lir(PinName pin) : capteur(pin) {}

    template <class Capteur, class Data>
    input<Capteur, Data>::input(PinName pin) : capteur(pin) {}

    Analog::Analog(PinName pin) : input(pin) {}

    Digital::Digital(PinName pin) : input(pin) {}
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
        lir1.read();
        lir2.read();
        lir3.read();
        lir4.read();
        lir5.read();
        lir6.read();
        lir7.read();
        lir8.read();
    }

    static bool _inverse { piste };
    
    bool inverse_read() {
        return _inverse;
    }

    void init(bool inverse) {
        _inverse = inverse;
    }

    Digital lirArray::lir1{LIR1};
    Digital lirArray::lir2{LIR2};
    Digital lirArray::lir3{LIR3};
    Digital lirArray::lir4{LIR4};
    Digital lirArray::lir5{LIR5};
    Digital lirArray::lir6{LIR6};
    Digital lirArray::lir7{LIR7};
    Digital lirArray::lir8{LIR8};

    lirArray array = lirArray();
}