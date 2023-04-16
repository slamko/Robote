#ifndef LIR_H
#define LIR_H

#include "config.h"

namespace LIR {

    enum PisteType {
        Noire = false,
        Blanche = true
    };

    static constexpr PisteType piste = PisteType::Blanche;

    bool inverse_read();

    template <class Capteur, class Data>
    class input {
    public:
        input(PinName pin);
        
        virtual Data read() = 0;

        Data read_cached() {
            return data;
        }

    protected:
        Capteur capteur;
        Data data;
    };

    class Analog : public input<AnalogIn, float> {
        public:
            Analog(PinName pin);

            float read() override {
                if (inverse_read()) {
                    data = (1.0f - capteur.read());
                } else {
                    data = capteur.read();
                }

                return data;
            }
    };

    class Digital : public input<DigitalIn, int> {
        public:
            Digital(PinName pin);

            int read() override {
                if (inverse_read()) {
                    data = !capteur.read();
                } else {
                    data = capteur.read();
                }

                return data;
            }
    };

    template <class T>
    class lir {
    public:
        lir(PinName pin);
        
        template <class Ret>
        Ret read();

        template<class Ret>
        operator Ret();
    private:
        T capteur;
    };

    struct lirArray {
        Digital lir1;
        Digital lir2;
        Digital lir3;
        Digital lir4;
        Digital lir5;
        Digital lir6;
        Digital lir7;
        Digital lir8;

        void read();
    };

    void init(bool inverse = piste);

    template <class In>
    template <class Ret>
    Ret lir<In>::read() {
        return capteur.read();
    }

    template <class In>
    template <class Ret>
    lir<In>::operator Ret() {
        return capteur.read_cached();
    }

    extern lir<Digital> lir1;
    extern lir<Digital> lir2;
    extern lir<Digital> lir3;
    extern lir<Digital> lir4;
    extern lir<Digital> lir5;
    extern lir<Digital> lir6;
    extern lir<Digital> lir7;
    extern lir<Digital> lir8;
}

#endif