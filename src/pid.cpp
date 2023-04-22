#include "mbed.h"
#include "include/config.h"
#include "include/pid.h"
#include "include/pont.h"
#include "include/lir.h"
#include "include/debug.h"
#include "include/sonore.h"

const float PID_Max_Out = 2.0f;
const float PID_Min_Out = -2.0f;
const float MAX_SPEED = 1.0f;

static float prev_pid_val;
static int8_t prev_error;
static int pid_integr;
static int pid_deriv;

static bool racourci = false;
static bool priorite = false;
static bool v_arrivee = false;
static bool arrivee = false;
static bool balise_gauche = false;
static bool balise_droite = false;

Timer sonore_run_timer;

namespace PID {

    static void pid_limit_out(float &out) {
        if (out <= PID_Min_Out) {
            out = PID_Min_Out;
        } else if (out >= PID_Max_Out) {
            out = PID_Max_Out;
        }
    }

    static int8_t pid_error() {
        using namespace LIR;

        DEBUG::fprint<int, int, int, int, int, int, int, int>(l1, l2, l3, l4, l5, l6, l7, l8);
        
        if (LIR::nul())  {
            if (prev_error > 0) {
                return 5;
            }
            if (prev_error < 0) {
                return -5;
            }
        }
        
        DEBUG::fprint<int, int, int, int, int, int, int, int>(l1, l2, l3, l4, l5, l6, l7, l8); 
        
        if (l4 &&  l5) return 0;

        if (l6 && !l7) return 1;
        if (l3 && !l2) return -1;
        if (l6 && l7 && !l8) return 2;
        if (l3 && l2) return -2;
        if (l7 && l8) return 3;
        if (l2 && l1) return -3;
        if (!l7 && l8) return 4;
        if (!l2 && l1) return -4;

        return 0;
    }

    static float pid_compute() {
        int8_t error = pid_error();

        DEBUG::print("error: %d\n\r", error);
        DEBUG::print("echo dist: %d\r\n", (int)(Sonore::get_obstacle_dist() * 1.0f));

        if (error > 4 ) {
            pid_deriv = +1;
            prev_pid_val = (KP_SP * error) + (KI * pid_integr) + (KD_SP * pid_deriv);
        } else if (error < -4) {
            pid_deriv = -1;
            prev_pid_val = (KP_SP * error) + (KI * pid_integr) + (KD_SP * pid_deriv);
        }
        else {
            pid_integr += error;
            pid_deriv = error - prev_error;
            prev_pid_val = (KP * error) + (KI * pid_integr) + (KD * pid_deriv); 
        }

        prev_error = error;
        //pid_limit_out(prev_pid_val);

        return prev_pid_val;
    }

    void calcul() {
        float pid_val = pid_compute(); 

        // si pid_val > 0 ==> tourner vers la gauche
        // si pid_val < 0 ==> tourner vers la droite
        if (!arrivee) {
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
        
    }

    void init() {       
    }
}