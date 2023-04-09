/*
#include "mbed.h"
#include "pinout.h"
#include "sonore.h"

#include <InterruptIn.h>
#include <chrono>
#include <cstdint>
#include <ratio>

//#define DEBUG
using namespace std;
using namespace chrono;

#define KP 0.20f // 0.22
#define KI 0.0f
#define KD 0.9f // 1.0

#define KP_SP 0.16f 
#define KI_SP 0.0f
#define KD_SP 0.75f 

DigitalOut led(LED);

// PID controller

const float PID_Max_Out = 2.0f;
const float PID_Min_Out = -2.0f;
const float MAX_SPEED = 1.0f;

const usec PID_Sample_Rate = 1000us; 

float prev_pid_val;
int8_t prev_error;
int pid_integr;
int pid_deriv;

Timer pid_timer;
//Ticker pid_tick;

// Pont H
constexpr int PWM_PERIOD = useci(100).count();

PwmOut ena(ENA);
PwmOut enb(ENB);
DigitalOut in1(IN1);
DigitalOut in2(IN2);
DigitalOut in3(IN3);
DigitalOut in4(IN4);

#pragma region Capteurs_Infra_Rouges

DigitalIn lir1(LIR1);
DigitalIn lir2(LIR2);
//
DigitalIn lir3(LIR3);
DigitalIn lir4(LIR4);
DigitalIn lir5(LIR5);
DigitalIn lir6(LIR6);
//
DigitalIn lir7(LIR7);
DigitalIn lir8(LIR8);

//AnalogIn lir2(LIR2);

AnalogIn lir3(LIR3);
AnalogIn lir4(LIR4);
AnalogIn lir5(LIR5);
AnalogIn lir6(LIR6);

//AnalogIn lir7(LIR7);

#pragma end_region Capteurs_Infra_Rouges


float lir(float l) {
    return (1.0f - l);
}

int lir(int l) {
    return !l;
}

static void pid_limit_out(float &out) {
    if (out <= PID_Min_Out) {
        out = PID_Min_Out;
    } else if (out >= PID_Max_Out) {
        out = PID_Max_Out;
    }
}

static int8_t pid_error() {

    #ifdef DEBUG
    printf("%d %d %d %d %d %d %d %d\n\r", lir(lir1), lir(lir2), lir(lir3), lir(lir4), lir(lir5), lir(lir6), lir(lir7), lir(lir8));
    #endif

   // if ((lir(lir6) || lir(lir7) || lir(lir8)) && (lir(lir3) || lir(lir2) || lir(lir1))) return 0;
    
    if (!lir(lir6) && !lir(lir7) && !lir(lir8) && !lir(lir5) && !lir(lir3) && !lir(lir2) && !lir(lir1) && !lir(lir4))  {
        if (prev_error > 0) {
            return 5;
        }
        if (prev_error < 0) {
            return -5;
        }
    }
#ifdef DEBUG
    printf("%d %d %d %d %d %d %d %d\n\r", lir(lir1), lir(lir2), lir(lir3), lir(lir4), lir(lir5), lir(lir6), lir(lir7), lir(lir8));
#endif 

    if (lir(lir4) &&  lir(lir5)) return 0;

    if (lir(lir6) && !lir(lir7)) return 1;
    if (lir(lir3) && !lir(lir2)) return -1;
    if (lir(lir6) && lir(lir7) && !lir(lir8)) return 2;
    if (lir(lir3) && lir(lir2)) return -2;
    if (lir(lir7) && lir(lir8)) return 3;
    if (lir(lir2) && lir(lir1)) return -3;
    if (!lir(lir7) && lir(lir8)) return 4;
    if (!lir(lir2) && lir(lir1)) return -4;

    return 0;
}

static float pid_compute() {

    if (pid_timer.elapsed_time() > PID_Sample_Rate) {
        int8_t error = pid_error();

#ifdef DEBUG
        printf("%d\n\r", error);
#endif
        
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
        pid_timer.reset();

        return prev_pid_val;
    }
    return prev_pid_val;
}

static void pid_control() {
    float pid_val = pid_compute(); // si pid_val >0 ==> tourner vers la gauche
                                    // si pid_val <0 ==> tourner vers la droite
    
    ena.write(abs(MAX_SPEED + pid_val)); // moteur droite

    if ((MAX_SPEED - pid_val) < 0.0f) {
        in3 = 0; // moteur gauche arriere
        in4 = 1;
    } else {
        in3 = 1;   // moteur gauche avant
        in4 = 0;
    }

    enb.write(abs(MAX_SPEED - pid_val)); // moteur gauche

    if ((MAX_SPEED + pid_val) < 0.0f) {
        in1 = 0; // moteur droit arriere
        in2 = 1;
    } else {
        in1 = 1; // moteur droit avant
        in2 = 0;
    }
    
}

static void config_ports() {

    pid_timer.start();
   // pid_tick.attach(&pid_control, PID_Sample_Rate);

    ena.period_us(PWM_PERIOD);
    ena.write(0);
    in1 = 1;
    in2 = 0;

    enb.period_us(PWM_PERIOD);
    enb.write(0);
    in3 = 1;
    in4 = 0;
}

int main()
{
    config_ports();
    //Sonore::sonore_init();

    while (1) {
        pid_control();
        //Sonore::sonore_ctrl();
        
        
       // ena.write((read_lir(lir5) / 3.0f) + (read_lir(lir6) / 4.0f) + (read_lir((float)lir7) / 5.0f));
        //enb.write((read_lir(lir4) / 3.0f) + (read_lir(lir3) / 4.0f) + (read_lir((float)lir2) / 5.0f));



        //wait_us(500000);
        
    }
}
*/