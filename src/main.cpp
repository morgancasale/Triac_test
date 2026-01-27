#include <Arduino.h>
#include "control_TRIAC/control_TRIAC.h"

TRIACController *heaterController;

void setup() {
  Serial.begin(9600);
  heaterController = new TRIACController(true);
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