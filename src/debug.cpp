#include "mbed.h"
#include <stdarg.h>
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

    template <> void add_format<int>(char *buf) {
        strcpy(buf + strlen(buf), "%d ");
    }

    template <> void add_format<int8_t>(char *buf) {
        strcpy(buf + strlen(buf), "%d ");
    }

    template <> void add_format<size_t>(char *buf) {
        strcpy(buf + strlen(buf), "%zu ");
    }

    template <> void add_format<float>(char *buf) {
        strcpy(buf + strlen(buf), "%f ");
    }

    template <> void add_format<double>(char *buf) {
        strcpy(buf + strlen(buf), "%lf ");
    }

    template <> void add_format<str>(char *buf) {
        strcpy(buf + strlen(buf), "%s ");
    }

    void fprint_iter(char *format) { 
        sprintf(format + strlen(format), "\r\n");
    }

    void d_assert(bool pred) {
        assert(pred);
    }
    
    #else 

    void d_assert(bool pred) { }

    void print(const char *msg, ...) {}

    #endif

    void init() {
    }
}