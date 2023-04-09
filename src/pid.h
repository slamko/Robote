#include "mbed.h"
#include "config.h"

#define KP 0.20f // 0.22
#define KI 0.0f
#define KD 0.9f // 1.0

#define KP_SP 0.16f 
#define KI_SP 0.0f
#define KD_SP 0.75f 

namespace PID {

    void calcul();

    void init();

}