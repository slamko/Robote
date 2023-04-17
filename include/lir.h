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

    class input_read {
        public:
            virtual void read() = 0;
    };

    template <class Capteur, class Data>
    class input {
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

    class Analog : public input<AnalogIn, float>, public input_read {
        public:
            Analog(PinName pin);
            void read() override;
    };

    class Digital : public input<DigitalIn, int>, public input_read {
        public:
            Digital(PinName pin);

            void read() override;
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
        static input_read *lir[8];
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

    extern lirArray array;
}

#endif