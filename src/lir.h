#ifndef LIR_H
#define LIR_H

#include "config.h"

namespace LIR {
    template <typename T>
    class lir {
    public:
        lir(PinName pin, bool inverse);
        
        template<typename Ret>
        Ret read();

        template<class Ret>
        operator Ret();
    private:
        bool inverse_read;
        T capteur;
    };

    template <class T>
    template <typename Ret>
    Ret lir<T>::read() {
        if (inverse_read) {
            return static_cast<Ret>(1.0f - static_cast<float>(capteur.read()));
        }
        return static_cast<Ret>(capteur.read());
    }

    template <>
    template <typename Ret>
    Ret lir<mbed::DigitalIn>::read() {
        if (inverse_read) {
            return static_cast<Ret>(!capteur.read());
        }
        return static_cast<Ret>(capteur.read());
    }

    template <>
    template <typename Ret>
    Ret lir<mbed::AnalogIn>::read() {
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
    lir<mbed::DigitalIn>::operator Ret() {
        return read<Ret>();
    }

    template <>
    template <class Ret>
    lir<mbed::AnalogIn>::operator Ret() {
        return read<Ret>();
    }


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