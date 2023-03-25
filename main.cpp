#include "mbed.h"


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
#define LIR2 A6
#define LIR3 A5
#define LIR4 A4
#define LIR5 A3
#define LIR6 A2
#define LIR7 A1
#define LIR8 A0

#define B D9
#define R D10
#define Y D11

#define OUTL D12
#define OUTR D2

#define LEDON D13

PwmOut ena(ENA);
PwmOut enb(ENB);
DigitalOut in1(IN1);
DigitalOut in2(IN2);
DigitalOut in3(IN3);
DigitalOut in4(IN4);

DigitalIn lir1(LIR1);
/*DigitalIn lir2(LIR2);
DigitalIn lir3(LIR3);
DigitalIn lir4(LIR4);
DigitalIn lir5(LIR1);
DigitalIn lir6(LIR2);
DigitalIn lir7(LIR3);*/
DigitalIn lir8(LIR4);

AnalogIn lir2(LIR2);
AnalogIn lir3(LIR3);
AnalogIn lir4(LIR4);
AnalogIn lir5(LIR1);
AnalogIn lir6(LIR2);
AnalogIn lir7(LIR3);


int main()
{
    ena.period_us(PWM_PERIOD);
    ena.write(1);
    in1 = 1;
    in2 = 0;

    enb.period_us(PWM_PERIOD);
    enb.write(1);
    in3 = 1;
    in4 = 0;

    while (1);
}
