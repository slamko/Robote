#include "mbed.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/move.h"
#include "include/debug.h"
#include "include/lir.h"
#include "include/rc5.h"
#include "include/events.h"
#include <stdio.h>

static void init() {
    H::init();
    Move::init();
    Telecommande::init();
    Sonore::init();
}

int main()
{
    init();
    
    while (1) {
        Telecommande::control();
        Move::control();   
    }
}
