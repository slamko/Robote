#include "mbed.h"
#include "pinout.h"
#include "sonore.h"
#include "pont.h"
#include "lir.h"
#include "pid.h"

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
        PID::compute();               
    }
}
