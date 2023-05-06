#ifndef LIR_H
#define LIR_H

#include "config.h"

namespace LIR {

    enum PisteType {
        Noire = false,
        Blanche = true
    };

    class input_read {
        public:
            virtual void read() = 0;
    };

    template <class Capteur, class Data>
    class input : public input_read, private NonCopyable<input<Capteur, Data>> {
    public:
        input(PinName pin);
        
        Data read_cached() {
            return data;
        }

        operator Data() {
            return read_cached();
        }

    protected:
        Capteur capteur;
        Data data;
    };

    class Analog : public input<AnalogIn, float>{
        public:
            using input::input;
            void read() override;
    };


    class Digital : public input<DigitalIn, int> {
        public:
            using input::input;
            void read() override;
    };

    void read();

    bool nul();

    bool tout();

    bool un();

    bool croisement();

    void init();

    extern Digital l1;
    extern Digital l2;
    extern Digital l3;
    extern Digital l4;
    extern Digital l5;
    extern Digital l6;
    extern Digital l7;
    extern Digital l8;
    extern Digital lg, ld;
}

#endif