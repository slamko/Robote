#include "mbed.h"
#include <cstdint>

#include "include/config.h"
#include "include/pid.h"
#include "include/pont.h"
#include "include/lir.h"
#include "include/debug.h"
#include "include/sonore.h"
#include "include/move.h"

const float PID_Max_Out = 2.0f;
const float PID_Min_Out = -2.0f;
const float MAX_SPEED = 1.0f;

static float prev_pid_val;
static int pid_integr;
static int pid_deriv;

namespace PID {

    static void limit_out(float &out) {
        if (out <= PID_Min_Out) {
            out = PID_Min_Out;
        } else if (out >= PID_Max_Out) {
            out = PID_Max_Out;
        }
    }

    static float pid_formule(int8_t error, float kp, float kd, float ki = 0.0f) {
        return (kp * error) + (ki * pid_integr) + (kd * pid_deriv);
    }

    static float pid_compute(const int8_t error, const int8_t prev_error) {
        float pid_val;

        DEBUG::print("error: %d \r\n", error);

        if (error >= 4 ) {
            pid_deriv = +1;
            pid_val = pid_formule(error, KP_SP, KD_SP);
        } else if (error < -4) {
            pid_deriv = -1;
            pid_val = pid_formule(error, KP_SP, KD_SP);
        }
        else {
            pid_integr += error;
            pid_deriv = error - prev_error;
            pid_val = pid_formule(error, KP, KD);
        }

        //limit_out(pid_val);

        return pid_val;
    }

    void calcul(const int8_t error, const int8_t prev_error) {
        float pid_val = pid_compute(error, prev_error); 

        // si pid_val > 0 ==> tourner vers la gauche
        // si pid_val < 0 ==> tourner vers la droite

        H::moteur_droit(abs(MAX_SPEED + pid_val)); 

        if ((MAX_SPEED - pid_val) < 0.0f) {
            H::moteur_gauche_arriere();
        } else {
            H::moteur_gauche_avant();
        }

        H::moteur_gauche(abs(MAX_SPEED - pid_val));

        if ((MAX_SPEED + pid_val) < 0.0f) {
            H::moteur_droit_arriere();
        } else {
            H::moteur_droit_avant();
        }
    }

    void init() {       
    }
}