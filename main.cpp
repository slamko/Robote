#include "mbed.h"
#include "PID.h"

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
#define KP_S 0.16f // 0.22
#define KI 0.0f
#define KD_S 0.75f 

#define PWM_PERIOD 100 //us

#define ENA D3
#define ENB D6

#define IN1 D4
#define IN2 D5
#define IN3 D8
#define IN4 D7

#define ECHO D1
#define TRIG D0

#define LIR1 D9 // LED INFRA ROURGE
#define LIR2 D10
#define LIR3 A6
#define LIR4 A0
#define LIR5 A3
#define LIR6 A2
#define LIR7 A1
#define LIR8 D11

#define B D9
#define R D10
#define Y D11

#define OUTL D12
#define OUTR D2

#define LED D13

typedef microseconds usec;
typedef duration<float, std::micro> usecf;


DigitalOut led(LED);

// PID controller

const float PID_Max_Out = 1.0f;
const float PID_Min_Out = 0.0f;
const usec PID_Sample_Rate = 1000us; 
Timer pid_timer;
float prev_pid_val;
const float MAX_SPEED = 1.0f;
int prev_error;
int last_error;
int pid_integr;
int pid_deriv;
//Ticker pid_tick;

// Pont H
PwmOut ena(ENA);
PwmOut enb(ENB);
DigitalOut in1(IN1);
DigitalOut in2(IN2);
DigitalOut in3(IN3);
DigitalOut in4(IN4);

// Capteur Sonore
const usec SONORE_RESTART_INT = 60000us; 
const usec TRIG_PULSE_DUR = 10us; 
const usecf SOUND_SPEED = 0.034us;

DigitalOut trig(TRIG);
InterruptIn echo(ECHO);
usec echo_pulse_dur;
Timer echo_timer;
Timer trig_timer;
Timer trig_repeat_timer;

#pragma region Capteurs_Infra_Rouges

DigitalIn lir1(LIR1);
DigitalIn lir2(LIR2);
///*
DigitalIn lir3(LIR3);
DigitalIn lir4(LIR4);
DigitalIn lir5(LIR5);
DigitalIn lir6(LIR6);
//*/
DigitalIn lir7(LIR7);
DigitalIn lir8(LIR8);

//AnalogIn lir2(LIR2);
/*
AnalogIn lir3(LIR3);
AnalogIn lir4(LIR4);
AnalogIn lir5(LIR5);
AnalogIn lir6(LIR6);
*/
#pragma end_region Capteurs_Infra_Rouges

//AnalogIn lir7(LIR7);

int lir(int l) {
    return !l;
}

static void gen_trig_pulse() {
    trig = 1;
    //printf("trig\n\r");
    trig_timer.start();
}

static void check_trig_pulse() {
    if (!trig) return;

    if (trig_timer.elapsed_time() >= TRIG_PULSE_DUR) {
        trig = 0;
        //printf("fin_trig\n\r");
        trig_timer.stop();
    }
}

static void sonore_ctrl() {
    check_trig_pulse();

    if (trig_repeat_timer.elapsed_time() > SONORE_RESTART_INT) {
        trig_repeat_timer.reset();
        gen_trig_pulse();
    }
}

static void run_sonore() {
    gen_trig_pulse();
}

static void on_echo_rise() {
    echo_timer.start();
   // printf("rise\n\r");
   // led = 1;
}

static void on_echo_fall() {
    echo_timer.stop();
    //printf("fall\n\r");
    //led = 0;
    echo_pulse_dur = echo_timer.elapsed_time();
}

static float get_obstacle_dist(usec pulse_dur) {
    return (SOUND_SPEED.count() * pulse_dur.count()) / 2.0f;
}

static void pid_set_output(float &out) {
    if (out <= -1.0f) {
        out = -1.0f;
    } else if (out >= 1.0f) {
        out = 1.0f;
    }
}

static int pid_error() {

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

    if (pid_timer.elapsed_time() > PID_Sample_Rate ) {
        int error = pid_error();
#ifdef DEBUG
        printf("%d\n\r", error);
#endif
        
        if (error > 4 ) {
            pid_deriv = +1;
            prev_pid_val = (KP_S * error) + (KI * pid_integr) + (KD_S * pid_deriv);
        } else if (error < -4) {
            pid_deriv = -1;
            prev_pid_val = (KP_S * error) + (KI * pid_integr) + (KD_S * pid_deriv);
        }
        else {
            pid_integr += error;
            pid_deriv = error - prev_error;
            prev_pid_val = (KP * error) + (KI * pid_integr) + (KD * pid_deriv); 
        }

        prev_error = error;
        //pid_set_output(prev_pid_val);
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
   /* controller.setInputLimits(0.0f, 3.3f);
    controller.setOutputLimits(0.0f, 1.0f);
    controller.setBias(0.3f);
    controller.setMode(1);*/

    echo.rise(&on_echo_rise);
    echo.fall(&on_echo_fall);
    echo.mode(PullDown);

    trig_repeat_timer.start();
    
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
    run_sonore();

    while (1) {
        sonore_ctrl();
        pid_control();

        
        
       // ena.write((read_lir(lir5) / 3.0f) + (read_lir(lir6) / 4.0f) + (read_lir((float)lir7) / 5.0f));
        //enb.write((read_lir(lir4) / 3.0f) + (read_lir(lir3) / 4.0f) + (read_lir((float)lir2) / 5.0f));



        //wait_us(500000);
        
    }
}
