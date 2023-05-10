#ifndef PID_H
#define PID_H

#include "mbed.h"

#define KP 0.20f // 0.20
#define KI 0.01f
#define KD 10.0f // 1.0

#define KP_URGENTE 0.2f // 0.16
#define KI_URGENTE 0.0f
#define KD_URGENTE 0.85f // 0.75

#define KP_IMPORTANTE 0.25f // 0.16
#define KI_IMPORTANTE 0.0f
#define KD_IMPORTANTE 0.85f // 0.75

#define KP_MAX 0.3f // 0.16
#define KI_MAX 0.0f
#define KD_MAX 0.85f // 0.75

namespace PID {

    void calcul(const int8_t error, const int8_t prev_error);

    void init();

}
#endif