#ifndef SONORE_H
#define SONORE_H

#include "config.h"

namespace Sonore {
    void control();
    void run_sonore();
    void init();

    void debug();

    void start();
    void stop();

    int no_obstacle_count();
    float get_obstacle_dist();
    bool obstacle_detected();
    bool obstacle_proche();
}

#endif