#ifndef R_SONORE 
#define R_SONORE 
#include "pinout.h"

namespace Sonore {
    void sonore_ctrl();

    void run_sonore();

    void sonore_init();

    float get_obstacle_dist(usec pulse_dur);
}

#endif