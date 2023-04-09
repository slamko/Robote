#include "mbed.h"
#include "include/lir.h"

namespace LIR {
    template <class T>
    lir<T>::lir(PinName pin, bool inverse) : capteur(pin), inverse_read(inverse) {}

    lir<DigitalIn> lir1{LIR1, PISTE_BLANCHE};
    lir<DigitalIn> lir2{LIR2, PISTE_BLANCHE};
    lir<DigitalIn> lir3{LIR3, PISTE_BLANCHE};
    lir<DigitalIn> lir4{LIR4, PISTE_BLANCHE};
    lir<DigitalIn> lir5{LIR5, PISTE_BLANCHE};
    lir<DigitalIn> lir6{LIR6, PISTE_BLANCHE};
    lir<DigitalIn> lir7{LIR7, PISTE_BLANCHE};
    lir<DigitalIn> lir8{LIR8, PISTE_BLANCHE};
}
