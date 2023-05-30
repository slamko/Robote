#include "mbed.h"
#include "include/sonore.h"
#include "include/pont.h"
#include "include/move.h"
#include "include/debug.h"
#include "include/lir.h"
#include "include/rc5.h"
#include "include/events.h"
#include <stdio.h>

//InterruptIn d(A4);
//bool rise = false;

static void init() {
    H::init();
    Move::init();
    //Telecommande::init();
    Sonore::init();
}

int main()
{
    //d.rise([]() {rise = true;});
    init();
    
    while (1) {
        /*if (rise) {
            printf("ererherher\r\n");
            rise = false;
        }*/
        Move::control();   
                
    }
}
