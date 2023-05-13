#include "mbed.h"
#include <cstdint>

#include "include/config.h"
#include "include/pid.h"
#include "include/pont.h"
#include "include/lir.h"
#include "include/debug.h"
#include "include/sonore.h"
#include "include/move.h"


namespace PID {
    const float MAX_SPEED = 1.0f;

    static float prev_pid_val;
    static int pid_integr;
    static int pid_deriv;

    static bool gauche_arriere = false; 
    static bool droit_arriere = false; 
    
    static float max_out = 0.9f;

    static float limit_out(float&& out) {
        if (out <= -max_out) {
            return -max_out;
        } else if (out >= max_out) {
            return  max_out;
        }
        return out;
    }

    static void limit_out(float &out) {
        out = limit_out(mstd::move(out));
    }

    void set_max_out(const float max_val) {
        max_out = max_val;
    }

    static float pid_formule(int8_t error, float kp, float kd, float ki = 0.0f) {
        return (kp * error) + (ki * pid_integr) + (kd * pid_deriv);
    }

    static float pid_compute(const int8_t error, const int8_t prev_error) {
        float pid_val;
        using namespace Move;

       // DEBUG::print("error: %d \r\n", error);

        if (error != prev_error) {
            pid_integr += error;
        }
        pid_deriv = error - prev_error;

        if (error == 0) {
            pid_integr = 0;
        }
        
        switch (abs(error)) {
        case Err::MIN:
            pid_val = pid_formule(error, KP_MIN, KD_MIN);
            break;
        case Err::MOYENNE:
            pid_val = pid_formule(error, KP_MOYENNE, KD_MOYENNE, KI);
            break;
        case Err::IMPORTANTE:
            pid_val = pid_formule(error, KP_IMPORTANTE, KD_IMPORTANTE, KI);
            break;
        case Err::MAX:
            pid_val = pid_formule(error, KP_MAX, KD_MAX, KI);
            break;
        case Err::URGENTE:
            if (error < 0) {
                pid_deriv = -1;
            } else {
                pid_deriv = 1;
            }
            pid_deriv = 0;
            pid_val = pid_formule(error, KP_URGENTE, KD_URGENTE);
            break;
        default:
            pid_val = pid_formule(error, KP, KD);
            break;
        }

        limit_out(pid_val);

        return pid_val;
    }

    void calcul(const int8_t error, const int8_t prev_error) {
        float pid_val = pid_compute(error, prev_error); 

        // si pid_val > 0 ==> tourner vers la gauche
        // si pid_val < 0 ==> tourner vers la droite
        
        if ((MAX_SPEED - pid_val) < 0.0f) {
            H::moteur_gauche_arriere();
        } else {
            H::moteur_gauche_avant();
        }

        if ((MAX_SPEED + pid_val) < 0.0f) {
            H::moteur_droit_arriere();
        } else {
            H::moteur_droit_avant();
        }


        H::moteur_droit(limit_out(abs(MAX_SPEED + pid_val))); 
        H::moteur_gauche(limit_out(abs(MAX_SPEED - pid_val)));
        
/*
        if (gauche_arriere) {
            if ((MAX_SPEED - pid_val) > 0.1f){
                H::moteur_gauche_avant();
            } 
        } else {
            if ((MAX_SPEED - pid_val) < 0.0f) {
                H::moteur_gauche_arriere();
                gauche_arriere = true;
            }  
        }

        if ((MAX_SPEED - pid_val) < 0.0f && !gauche_arriere) {
            H::moteur_gauche_arriere();
            gauche_arriere = true;
        } else if ((MAX_SPEED - pid_val) > 0.0f && !gauche_arriere){
            H::moteur_gauche_avant();
        } 
        if ((MAX_SPEED - pid_val) < 0.1f && gauche_arriere){
            H::moteur_gauche_arriere();
        }

        H::moteur_gauche(abs(MAX_SPEED - pid_val));

        if ((MAX_SPEED + pid_val) < 0.0f) {
            H::moteur_droit_arriere();
        } else {
            H::moteur_droit_avant();
        }*/
    }

    void init() {       
    }
}