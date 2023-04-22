#include "mbed.h"
#include <stdarg.h>
#include <stdlib.h>
#include "include/debug.h"
#include "include/config.h"
#include "include/events.h"

namespace DEBUG {

    #ifdef DEBUG_MODE

    void print(const char *msg, ...) {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }

    template <>
    void fprint(const char *msg) {
        printf(msg);
    }

    #else 

    void print(const char *msg, ...) {}

    template <>
    void fprint(const char *msg) {}

    #endif

    

    void init() {
    }
}