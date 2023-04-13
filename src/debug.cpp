#include "mbed.h"
#include <stdarg.h>
#include <stdlib.h>

namespace DEBUG {
    static EventQueue queue(4 * EVENTS_EVENT_SIZE);\
    static Thread debug_t;

    void print(const char *msg, ...) {
        #ifdef DEBUG_MODE

        va_list args;
        va_start(args, msg);

        vfprintf(msg);
        va_end(args);

        #endif
    }

    void nb_print(const char *msg) {
        #ifdef DEBUG_MODE

        queue.call(&print, msg);
        
        #endif
    }

    void init() {
        debug_t.start(callback(&queue, &EventQueue::dispatch_forever));
    }
}