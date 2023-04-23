#ifndef DEBUG_H
#define DEBUG_H

#include "mbed.h"
#include "include/config.h"
#include "include/events.h"

namespace DEBUG {
    void print(const char *msg, ...);

    #ifdef DEBUG_MODE

    void fprint(int num);

    void fprint(float num);

    void fprint(double num);

    void fprint(const char *msg);

    template <class Acc, class Sec, class ...Args>
    void fprint(Acc acc, Sec sec, Args ...args) {
        fprint(acc);
        fprint(sec, args...);
    } 

    template <class ...Args>
    void nb_print(const char *msg, Args ...args) {
       Events::call(printf, msg, args...);
    }

    #else
    template <class ...Args>
    void nb_print(const char *msg, Args ...args) { }
    
    template <class Msg>
    void fprint(Msg msg) { }

    template <class Acc, class Sec, class ...Args>
    void fprint(Acc acc, Sec sec, Args ...args) { } 
    #endif    


    void init();
}

#endif