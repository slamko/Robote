#include "mbed.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/move.h"
#include "include/debug.h"
#include "include/lir.h"
#include "include/events.h"

static void init() {
    Sonore::init();
    H::init();
    Move::init();
}

int main()
{
    init();

    while (1) {
        Move::control();
       // Events::loop();            
    }
}
