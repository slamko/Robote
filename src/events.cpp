#include "mbed.h"
#include "include/events.h"
#include <memory>
#include <vector>
#include <functional>

namespace Events {
    EventQueue queue {};
    static Thread ethread {};

    void init() {
        ethread.start(callback(&queue, &EventQueue::dispatch_forever));
    }
}