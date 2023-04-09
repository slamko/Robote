#include "lir.h"

namespace LIR {
    lir::lir(PinName pin, bool inverse) : capteur(pin) , inverse_read(inverse) {}

    template <class R>
    R lir::read() {
        if (inverse_read) {
            return static_cast<R>(1.0f - static_cast<float>(capteur.read()));
        }
        return static_cast<R>(capteur.read());
    }

    lir lir1{LIR1, true};
    lir lir2{LIR2, true};
    lir lir3{LIR3, true};
    lir lir4{LIR4, true};
    lir lir5{LIR5, true};
    lir lir6{LIR6, true};
    lir lir7{LIR7, true};
    lir lir8{LIR8, true};
}
