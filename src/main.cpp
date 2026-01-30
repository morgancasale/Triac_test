#include <Arduino.h>
#include "config.h" 
#include "FTH_controller/FTH_controller.h"
#include "Thermistor/Thermistor.h"

//TRIACController *heaterController;
Thermistor *heaterTempSensor;

void setup() {
  Serial.begin(9600);
  //heaterController = new TRIACController(true);
  heaterTempSensor = new Thermistor(
    HEATER_T_SENS_PIN,
    HEATER_T_SENS_VCC,
    HEATER_T_SENS_ANALOG_REFERENCE,
    HEATER_T_SENS_ADC_MAX,
    HEATER_T_SENS_SERIES_RESISTOR,
    HEATER_T_SENS_NOMINAL_RESISTANCE,
    HEATER_T_SENS_NOMINAL_TEMPERATURE,
    HEATER_T_SENS_B_COEFFICIENT,
    HEATER_T_SENS_SAMPLES,
    HEATER_T_SENS_SAMPLES_INTERVAL
  );
  FTH_controller_setup();
}

void serialFlush(){
  while(Serial.available() > 0) {
    Serial.read();
  }
}

void loop() {
  heaterController->loop();

  if(Serial.available()){
    uint8_t power_level = Serial.parseInt();
    serialFlush();
    if(heaterController->set_power_level(power_level)){
      Serial.print("Power level set to: ");
      Serial.println(power_level);
    }
    else{
      Serial.println("Invalid power level!");
    }
  }
  delay(1000);
}