#include "mbed.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/pid.h"
#include "include/lir.h"

static void init() {
    LIR::init();
    Sonore::init();
    H::init();
    PID::init();
}

int main()
{
    init();

    while (1) {
        Sonore::control();
        PID::calcul();               
    }
}
