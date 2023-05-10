#include "mbed.h"
#include "include/config.h"
#include "include/rc5.h"
#include "include/move.h"

namespace Telecommande {
   // static InterruptIn signal {RC5};

    void init() {
       /* signal.mode(PullNone);
        signal.rise(&Move::mise_en_marche);
        signal.fall(&Move::stop);*/
    }
}