#include "mbed.h"
#include <InterruptIn.h>
#include <chrono>

#define KP 0.5f
#define KI 0.2f
#define KD 0.2f

#define PWM_PERIOD 100

#define ENA D3
#define ENB D6

#define IN1 D4
#define IN2 D5
#define IN3 D8
#define IN4 D7

#define ECHO D1
#define TRIG D0

#define LIR1 A7 // LED INFRA ROURGE
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

#define LEDON D13

typedef std::chrono::microseconds usec;

// Pont H
PwmOut ena(ENA);
PwmOut enb(ENB);
DigitalOut in1(IN1);
DigitalOut in2(IN2);
DigitalOut in3(IN3);
DigitalOut in4(IN4);
DigitalOut ledon(LEDON);

// Capteur Sonor
DigitalOut trig(TRIG);
InterruptIn echo(ECHO);
usec echo_pulse_dur;
Timer echo_timer;
Timer trig_timer;
Timer trig_repeat_timer;

#pragma region Capteurs_Infra_Rouges

DigitalIn lir1(LIR1);
DigitalIn lir2(LIR2);
/*DigitalIn lir3(LIR3);
DigitalIn lir5(LIR5);
DigitalIn lir4(LIR4);
DigitalIn lir6(LIR6);*/
DigitalIn lir7(LIR7);
DigitalIn lir8(LIR8);

//AnalogIn lir2(LIR2);
AnalogIn lir3(LIR3);
AnalogIn lir4(LIR4);
AnalogIn lir5(LIR5);
AnalogIn lir6(LIR6);

#pragma end_region Capteurs_Infra_Rouges

float ecart;
float ecart_presced;
//AnalogIn lir7(LIR7);

static void gen_trig_pulse() {
    trig = 1;
    trig_timer.start();
}

static void check_trig_pulse() {
    if (!trig) return;

    if (trig_timer.elapsed_time() >= 10) {
        trig = 0;
        trig_timer.stop();
    }
}

static void sonor_ctrl() {
    
    if (trig_repeat_timer.elapsed_time().count() > )
    gen_trig_pulse();
    
}

static void on_echo_rise() {
    echo_timer.start();
}

static void on_echo_fall() {
    echo_timer.stop();
    
    echo_pulse_dur = echo_timer.elapsed_time().count();
}

static void config_ports() {
    echo.rise(&on_echo_rise);
    echo.fall(&on_echo_fall);
    echo.mode(PullDown);

    ena.period_us(PWM_PERIOD);
    ena.write(0);
    in1 = 1;
    in2 = 0;

    enb.period_us(PWM_PERIOD);
    enb.write(0);
    in3 = 1;
    in4 = 0;
}

float read_lir(float lir) {
    return lir;
}

int main()
{
    config_ports();

    while (1) {
        check_trig_pulse();

       // printf("%d %d %d %d %d %d\n\r", (int)(lir2 * 10), (int)(lir3 * 10), (int)(lir4 * 10), (int)(lir5 * 10), (int)(lir6 * 10), (int)(lir7 * 10));
        
       // ena.write((read_lir(lir5) / 3.0f) + (read_lir(lir6) / 4.0f) + (read_lir((float)lir7) / 5.0f));
        //enb.write((read_lir(lir4) / 3.0f) + (read_lir(lir3) / 4.0f) + (read_lir((float)lir2) / 5.0f));



        //wait_us(500000);
        
    }
}
