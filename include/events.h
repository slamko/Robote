#ifndef EVENTS_H
#define EVENTS_H

#include "mbed.h"

namespace Events {
    typedef int(*print_cb)(const char*, ...);
/*
    struct string_literal
    {
        const char *c_str() const noexcept { return _str; }
        
    private:
        const char *_str;
        constexpr string_literal(const char *str) noexcept : _str(str) {}

        friend string_literal operator""_s(const char *str, size_t len);
    };
*/
    void init();
    void loop();

    void print(print_cb cb, const char *msg);

/*
    template <typename F, typename ...Args>
    int call(F f, Args ...args) {
        return queue.call(f, args...);
    }
*/
}

#endif