#ifndef OUTIL_H
#define OUTIL_H

#include "mbed.h"

namespace Outil {
    template <class Rep, class Period>
    bool at_time(Timer &timer, std::chrono::duration<Rep, Period> &dur) {
        return timer.elapsed_time() >= dur; 
    }

    template <class T>
    bool at_time(Timer &timer, T duration) {
        return (timer.elapsed_time() >= duration);
    }
}


using usec = std::chrono::microseconds;
using usecf = std::chrono::duration<float, std::micro>;
using useci = std::chrono::duration<unsigned int, std::micro>;
using nanoi = std::chrono::duration<unsigned int, std::nano>;

#endif