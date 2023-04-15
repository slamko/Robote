#ifndef SONORE_H
#define SONORE_H

#include "config.h"

namespace Sonore {
    void control();

    void run_sonore();

    void init();

    float get_obstacle_dist();

    bool obstacle_detected();
}

#endif