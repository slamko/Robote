#include "mbed.h"
#include "include/pont.h"
#include "include/config.h"
#include "include/outils.h"

namespace H {
    static const int PWM_PERIOD = useci{100}.count();
    static const int ARRET_TIME = useci{250000}.count();
    static const int ARRIVEE_TIME = useci{50000}.count();

    static PwmOut enb {ENB};
    static PwmOut ena {ENA};
    static DigitalOut in1 {IN1};
    static DigitalOut in2 {IN2};
    static DigitalOut in3 {IN3};
    static DigitalOut in4 {IN4};

    float moteur_droit(float val) {
        enb.write(val);
        return val;
    }

    float moteur_gauche(float val) {
        ena.write(val);
        return val;
    }

    void moteurs(float val) {
        moteur_droit(val);
        moteur_gauche(val);
    }

    void moteur_droit_arriere() {
        in3 = 0;
        in4 = 1;
    }

    bool droit_arriere() {
        return in2;
    }

    void moteur_droit_avant() {
        in3 = 1;
        in4 = 0;
    }

    void moteur_gauche_arriere() {
        in1 = 0;
        in2 = 1;
    }

    bool gauche_arriere() {
        return in2;
    }
    
    void moteur_gauche_avant() {
        in1 = 1;
        in2 = 0;
    }

    void arret_motors() {
        in1 = 0;
        in2 = 0;
        in3 = 0;
        in4 = 0;
        H::moteur_droit(0.0f);
        H::moteur_gauche(0.0f);
    }

    void marche() {
        moteur_droit_avant();
        moteur_gauche_avant();
    }

    void marche_arriere() {
        moteur_droit_arriere();
        moteur_gauche_arriere();
        moteurs(1.0f);
    }

    void arret() {
        marche_arriere();
        
        wait_us(ARRET_TIME);
        arret_motors();
    }

    void arrivee() {
        marche_arriere();
        
        wait_us(ARRIVEE_TIME);
        arret_motors();
    }


    void init() {   
        enb.period_us(PWM_PERIOD);
        enb.write(0);

        ena.period_us(PWM_PERIOD);
        ena.write(0);
    }
}