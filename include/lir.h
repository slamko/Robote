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
    class input : public input_read  {
    public:
        input(PinName pin);
        
        Data read_cached() {
            return data;
        }

        operator Data() {
            return read_cached();
        }

        input(const input&) = delete;
        input& operator =(const input&) = delete;

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

    void init(bool inverse = piste);

    extern Digital lir1;
    extern Digital lir2;
    extern Digital lir3;
    extern Digital lir4;
    extern Digital lir5;
    extern Digital lir6;
    extern Digital lir7;
    extern Digital lir8;
}

#endif