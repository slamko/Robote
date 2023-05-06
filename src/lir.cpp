#include "mbed.h"
#include "include/config.h"
#include "include/lir.h"

namespace LIR {
    static constexpr PisteType piste = PisteType::Blanche;
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
        if (inverse_read()) {
            data = (1.0f - capteur.read());
        } else {
            data = capteur.read();
        }
    }

    void Digital::read() {
        if (inverse_read()) {
            data = !capteur.read();
        } else {
            data = capteur.read();
        }
    }

    void read() {
        l1.read();
        l2.read();
        l3.read();
        l4.read();
        l5.read();
        l6.read();
        l7.read();
        l8.read();
       // lg.read();
      //  ld.read();
    }

    bool nul() {
        return (!l1 && !l2 && !l3 && !l4 && !l5 && !l6 && !l7 && !l8);
    }

    bool tout() {
        return (l1 && l2 && l3 && l4 && l5 && l6 && l7 && l8);
    }

    bool un() {
        return (l1 || l2 || l3 || l4 || l5 || l6 || l7 || l8);
    }

    bool croisement() {
        return (l2 && l3 && l6 && l7);
    }

    Digital l1{LIR1};
    Digital l2{LIR2};
    Digital l3{LIR3};
    Digital l4{LIR4};
    Digital l5{LIR5};
    Digital l6{LIR6};
    Digital l7{LIR7};
    Digital l8{LIR8};
    
 //   Digital lg{LG}, ld {LD};
}