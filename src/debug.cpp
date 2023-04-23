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

    void fprint(const char *msg) {
        puts(msg);
    }

    void fprint(int num) {
        printf("%d ", num);
    }

    void fprint(float num) {
        printf("%f ", num);
    }

    void fprint(double num) {
        printf("%lf ", num);
    }

    #else 

    void print(const char *msg, ...) {}

    #endif

    void init() {
    }
}