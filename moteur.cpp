#include "mbed.h"
#include "pont.h"
#include "pinout.h"


static const int PWM_PERIOD = useci(100).count();

static PwmOut ena(ENA);
static PwmOut enb(ENB);
static DigitalOut in1(IN1);
static DigitalOut in2(IN2);
static DigitalOut in3(IN3);
static DigitalOut in4(IN4);


namespace H {

    float moteur_droit(float val) {
        ena.write(val);
        return val;
    }

    float moteur_gauche(float val) {
        enb.write(val);
        return val;
    }

    void moteur_droit_arriere() {
        in1 = 0;
        in2 = 1;
    }

    void moteur_droit_avant() {
        in1 = 1;
        in2 = 0;
    }

    void moteur_gauche_arriere() {
        in3 = 0;
        in4 = 1;
    }

    void moteur_gauche_avant() {
        in3 = 1;
        in4 = 0;
    }

    void init() {   
        ena.period_us(PWM_PERIOD);
        ena.write(0);
        moteur_droit_avant();

        enb.period_us(PWM_PERIOD);
        enb.write(0);
        moteur_gauche_avant();
    }
}