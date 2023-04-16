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

    lir<Digital> lir1{LIR1};
    lir<Digital> lir2{LIR2};
    lir<Digital> lir3{LIR3};
    lir<Digital> lir4{LIR4};
    lir<Digital> lir5{LIR5};
    lir<Digital> lir6{LIR6};
    lir<Digital> lir7{LIR7};
    lir<Digital> lir8{LIR8};
}