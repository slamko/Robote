#include "mbed.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/move.h"
#include "include/debug.h"
#include "include/lir.h"

static void init() {
    LIR::init();
    Sonore::init();
    H::init();
    Move::init();
    DEBUG::init();
}

int main()
{
    init();

    while (1) {
        Sonore::control();
        Move::control();               
    }
}
