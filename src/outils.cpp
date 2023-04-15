#include <mbed.h>

namespace Outil {
    template <class Rep, class Period>
    bool at_time(Timer &timer, std::chrono::duration<Rep, Period> &dur) {
        return timer.elapsed_time() >= dur; 
    }

    template <class T>
    bool at_time(Timer &timer, T duration) {
        return timer.elapsed_time().count() >= duration;
    }

}