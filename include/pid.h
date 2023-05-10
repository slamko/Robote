#ifndef PID_H
#define PID_H

#include "mbed.h"

#define KP 0.19f // 0.20
#define KI 0.0f
#define KD 10.0f // 1.0

#define KP_URGENTE 0.5f // 0.16
#define KI_URGENTE 0.0f
#define KD_URGENTE 0.0f

#define KP_MIN 0.19f // 0.16
#define KI_MIN 0.0f
#define KD_MIN 10.0f // 0.75

#define KP_MOYENNE 0.195f // 0.16
#define KI_MOYENNE 0.0f
#define KD_MOYENNE 10.0f // 0.75

#define KP_IMPORTANTE 0.3f // 0.16
#define KI_IMPORTANTE 0.0f
#define KD_IMPORTANTE 10.0f // 0.75

#define KP_MAX 0.4f // 0.16
#define KI_MAX 0.0f
#define KD_MAX 10.0f // 0.75

namespace PID {

    void calcul(const int8_t error, const int8_t prev_error);

    void init();

}
#endif