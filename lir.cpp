#include "lir.h"

namespace LIR {
    template <class T>
    lir<T>::lir(PinName pin, bool inverse) : capteur(pin), inverse_read(inverse) {}

    template <class T>
    template <class Ret>
    Ret lir<T>::read() {
        if (inverse_read) {
            return static_cast<Ret>(1.0f - static_cast<float>(capteur.read()));
        }
        return static_cast<Ret>(capteur.read());
    }

    template <>
    template <class Ret>
    Ret lir<DigitalIn>::read() {
        if (inverse_read) {
            return static_cast<Ret>(!capteur.read());
        }
        return static_cast<Ret>(capteur.read());
    }

    template <>
    template <class Ret>
    Ret lir<AnalogIn>::read() {
        if (inverse_read) {
            return static_cast<Ret>(1.0f - capteur.read());
        }
        return static_cast<Ret>(capteur.read());
    }

    template <class T>
    template <class Ret>
    lir<T>::operator Ret() {
        return read<Ret>();
    }

    template <>
    template <class Ret>
    lir<DigitalIn>::operator Ret() {
        return read<Ret>();
    }

    template <>
    template <class Ret>
    lir<AnalogIn>::operator Ret() {
        return read<Ret>();
    }

    lir<DigitalIn> lir1{LIR1, true};
    lir<DigitalIn> lir2{LIR2, true};
    lir<DigitalIn> lir3{LIR3, true};
    lir<DigitalIn> lir4{LIR4, true};
    lir<DigitalIn> lir5{LIR5, true};
    lir<DigitalIn> lir6{LIR6, true};
    lir<DigitalIn> lir7{LIR7, true};
    lir<DigitalIn> lir8{LIR8, true};
}
