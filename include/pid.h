#ifndef PID_H
#define PID_H

#define KP 0.22f // 0.22
#define KI 0.0f
#define KD 0.90f // 1.0

#define KP_SP 0.18f // 0.16
#define KI_SP 0.0f
#define KD_SP 0.75f // 0.75

namespace PID {

    void calcul();

    void init();

}
#endif