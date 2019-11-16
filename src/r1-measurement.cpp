#include <Arduino.h>

#include "r1-measurement.h"

R1Measurement::R1Measurement() {
  pinMode(CONTROL, OUTPUT);
  digitalWrite(CONTROL, LOW);
}

void R1Measurement::update() {
  if (millis() % 1000 == 0) {
    float vin; //a0 measurement
    float vout; // a1 measurement

    digitalWrite(CONTROL, HIGH);
    delay(1);

    /**
     *                 R2
     * Vout = Vin * ---------
     *              R1 + R2 
     * 
     * R1 = R2 * (Vin / Vout – 1)
     */

    vin = _vref * ((float)analogRead(VIN) / 1024.0);
    //Serial.println(vin, 6);
    vout = _vref * ((float)analogRead(VOUT) / 1024.0);
    //Serial.println(vout, 6);
    _r1 = ((vin / vout) - 1) * _r2;
    //Serial.println(r1, 6);
    
    digitalWrite(CONTROL, LOW);
  }
}

float R1Measurement::get() {
  return _r1;
}
