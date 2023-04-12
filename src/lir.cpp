#include "mbed.h"
#include "include/config.h"
#include "include/lir.h"

namespace LIR {
    template <class T>
    lir<T>::lir(PinName pin) : capteur(pin) {}

    static bool _inverse_read { piste };
    
    bool inverse_read() {
        return _inverse_read;
    }

    void init(bool inverse) {
        _inverse_read = inverse;
    }

    lir<DigitalIn> lir1{LIR1};
    lir<DigitalIn> lir2{LIR2};
    lir<DigitalIn> lir3{LIR3};
    lir<DigitalIn> lir4{LIR4};
    lir<DigitalIn> lir5{LIR5};
    lir<DigitalIn> lir6{LIR6};
    lir<DigitalIn> lir7{LIR7};
    lir<DigitalIn> lir8{LIR8};
}
