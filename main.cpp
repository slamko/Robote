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

void inter() {
    
}

int main()
{
    init();
    
    while (1) {
        
       /* if (rise) {
            printf("ererherher %d\r\n", (int)d);
            rise = false;
        }
        */
        //Telecommande::debug();
       // wait_us(10000);
        Move::control();   
                
    }
}
