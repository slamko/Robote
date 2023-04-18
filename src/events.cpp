#include "mbed.h"
#include "include/events.h"
#include <memory>
#include <vector>
#include <functional>

namespace Events {
    static std::vector<std::function<void()>> events(4, nullptr);
    static size_t event_num = 0;

    void loop() {
        if (event_num == 0) return;

        for (size_t e = 0; e < event_num; e++) {
            if (events[e]) events[e]();
        }
        event_num = 0;
    }

    void init() {
        //event_thread.start(&loop);
    }

    void call(const std::function<void()> &callback) {
        if (event_num >= events.size()) {
            events.push_back(callback);
        } else {
            events.at(event_num) = callback;
        }
        event_num++;
    }
}