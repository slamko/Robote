#ifndef R_SONORE 
#define R_SONORE 
#include "config.h"

namespace Sonore {
    void control();

    void run_sonore();

    void init();

    float get_obstacle_dist();

    bool obstacle_present();
}

#endif