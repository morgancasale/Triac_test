#ifndef FTH_controller_H
#define FTH_controller_H

#include <PID_v1.h>
#include "../config.h"
#include "../control_TRIAC/control_TRIAC.h"

#define HEATER_T_SENS_PIN A0

#define Kp 2.0
#define Ki 5.0
#define Kd 1.0

#define SETPOINT_TEMPERATURE 40.0

extern double Setpoint, Input, Output;

extern TRIACController *heaterController;
extern PID FTH_controller;

void FTH_controller_setup();

void FTH_controller_loop();

void start_FTH_controller();

void stop_FTH_controller();

bool set_FTH_setpoint(double sp);

#endif // FTH_controller_H
