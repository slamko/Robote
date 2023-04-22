#ifndef EVENTS_H
#define EVENTS_H

#include "mbed.h"

namespace Events {
    extern EventQueue queue;

    void init();

    template <typename F, typename ...Args>
    int call(F f, Args ...args) {
        return queue.call(f, args...);
    }
}

#endif