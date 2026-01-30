#include "FTH_controller.h"

double Setpoint, Input, Output;

TRIACController *heaterController;
PID FTH_controller(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void FTH_controller_setup(){
    heaterController = new TRIACController(false);

    Input = analogRead(HEATER_T_SENS_PIN);
    Setpoint = SETPOINT_TEMPERATURE; //Starting target temperature

    FTH_controller.SetMode(AUTOMATIC);
    FTH_controller.SetOutputLimits(0, TRIAC_MAX_PL); // Output limits correspond to FTH power levels
}

void FTH_controller_loop(){
    // Read temperature sensor
    Input = analogRead(HEATER_T_SENS_PIN);

    // Compute PID
    if (FTH_controller.Compute()) {
        // Set new power level
        heaterController->set_power_level(static_cast<uint8_t>(Output));
    }
}

void start_FTH_controller(){
    FTH_controller.SetMode(AUTOMATIC);
}

void stop_FTH_controller(){
    FTH_controller.SetMode(MANUAL);
    Output = 0;
    heaterController->set_power_level(static_cast<uint8_t>(Output)); // Turn off heating
}

bool set_FTH_setpoint(double sp){
    if(sp < 0 || sp > 100) return false; // Invalid setpoint
    Setpoint = sp;
    return true;
}