#include "mbed.h"
#include <stdarg.h>
#include <stdlib.h>
#include "include/debug.h"
#include "include/config.h"

namespace DEBUG {

    #ifdef DEBUG_MODE

    void print(const char *msg, ...) {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }

    #else 

    void print(const char *msg, ...) {}

    #endif

    void nb_print(const char *msg) {
       
    }

    void init() {
    }
}