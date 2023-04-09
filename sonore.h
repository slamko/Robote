#ifndef R_SONORE 
#define R_SONORE 
#include "pinout.h"

namespace Sonore {
    void control();

    void run_sonore();

    void init();

    float get_obstacle_dist(usec pulse_dur);
}

#endif