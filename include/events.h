#ifndef EVENTS_H
#define EVENTS_H

#include "mbed.h"

namespace Events {
    typedef int(*print_cb)(const char*, ...);

    void init();
    void loop();

    void print(print_cb cb, const char *msg);

/*
    template <typename F, typename ...Args>
    int call(F f, Args ...args) {
        return queue.call(f, args...);
    }
*/
}

#endif