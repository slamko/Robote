#include "mbed.h"
#include "string.h"
#include "include/events.h"

namespace Events {
    //EventQueue queue {};
    //static Thread ethread {};

    static const size_t PRINT_CB_NUM = 8;

    static print_cb print_cbs[PRINT_CB_NUM];
    static const char *print_cb_args[PRINT_CB_NUM];
    static size_t cur_cb = 0;

    void print(print_cb cb, const char *msg) {
        if (cur_cb >= PRINT_CB_NUM) return;

        print_cbs[cur_cb] = cb;
        print_cb_args[cur_cb] = msg;
        cur_cb++;
    }

    void loop() {
        if (cur_cb == 0) return;

        for (size_t cb = 0; cb < cur_cb; cb++) {
            print_cbs[cb](print_cb_args[cb]);
        }
        cur_cb = 0;
    }

    void init() {
       // ethread.start(callback(&queue, &EventQueue::dispatch_forever));
    }
}