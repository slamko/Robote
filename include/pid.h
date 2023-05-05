#ifndef PID_H
#define PID_H

#include "mbed.h"

#define KP 0.20f // 0.22
#define KI 0.0f
#define KD 10.0f // 1.0

#define KP_SP 0.18f // 0.16
#define KI_SP 0.0f
#define KD_SP 0.85f // 0.75

namespace PID {

    void calcul(const int8_t error, const int8_t prev_error);

    void init();

}
#endif