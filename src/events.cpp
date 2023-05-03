#include "mbed.h"
#include "include/events.h"

namespace Events {
    //EventQueue queue {};
    static Thread ethread {};

    void init() {
       // ethread.start(callback(&queue, &EventQueue::dispatch_forever));
    }
}