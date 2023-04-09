#include "mbed.h"
#include "include/config.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/lir.h"
#include "include/pid.h"

static void init() {
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
