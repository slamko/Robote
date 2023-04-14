#ifndef R_PINOUT
#define R_PINOUT

#include "mbed.h"

#define ENA D3
#define ENB D6

#define IN1 D4
#define IN2 D5
#define IN3 D8
#define IN4 D7

#define ECHO D1
#define TRIG D0

#define LIR1 D9  // LED INFRA ROURGE
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

using usec = std::chrono::microseconds;
using usecf = std::chrono::duration<float, std::micro>;
using useci = std::chrono::duration<unsigned int, std::micro>;
using nanoi = std::chrono::duration<unsigned int, std::nano>;

#define PISTE_BLANCHE true

#endif