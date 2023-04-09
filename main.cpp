#include "mbed.h"
#include "src/config.h"
#include "src/sonore.h"
#include "src/pont.h"
#include "src/lir.h"
#include "src/pid.h"

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
