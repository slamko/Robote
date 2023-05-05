#ifndef DEBUG_H
#define DEBUG_H

#include "mbed.h"
#include "include/config.h"
#include "include/events.h"

using str = const char *;

namespace DEBUG {
    void print(const char *msg, ...);

    #ifdef DEBUG_MODE

    template <class T> 
    void add_format(char *);

    void fprint_iter(char *format);

    template <class T, class ...Args> 
    void fprint_iter(char *format, T, Args ...args) {
        add_format<T>(format);
        fprint_iter(format, args...);
    }

    template <class ...Args> 
    void fprint(Args&& ...args) {
        char msg[(sizeof...(args) * 4u) + 3u] = {0};
        fprint_iter(msg, args...);
        printf(msg, args...);
    }

    template <class T, class ...Args>
    void fprint(Args&& ...args) {
        fprint(static_cast<T>(args)...);
    }

    template <class ...Args>
    void nb_print(const char *msg) {
       Events::print(printf, msg);
    }

    #else
    template <class ...Args>
    void nb_print(const char *msg, Args ...args) { }
    
    template <class Msg>
    void fprint(Msg msg) { }

    template <class T, class ...Args>
    void fprint(Args&& ...args);

    template <class T, class ...Args>
    void fprint(Args&& ...args) {
    }
    #endif    


    void init();
}

#endif