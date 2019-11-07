#include <Arduino.h>

#define VIN A0
#define VOUT A1
#define CONTROL 10

static const float vref = 5.0;
static float vin; //a0 measurement
static float vout; // a1 measurement
static float r1;
static const float r2 = 1000.0;

void setupR1() {
  pinMode(CONTROL, OUTPUT);
  digitalWrite(CONTROL, LOW);
}

void updateR1() {
  if (millis() % 1000 == 0) {
    digitalWrite(CONTROL, HIGH);
    delay(1);

    /**
     *                 R2
     * Vout = Vin * ---------
     *              R1 + R2 
     * 
     * R1 = R2 * (Vin / Vout – 1)
     */

    vin = vref * ((float)analogRead(VIN) / 1024.0);
    //Serial.println(vin, 6);
    vout = vref * ((float)analogRead(VOUT) / 1024.0);
    //Serial.println(vout, 6);
    r1 = ((vin / vout) - 1) * r2;
    //Serial.println(r1, 6);
    
    digitalWrite(CONTROL, LOW);
  }
}

float getR1() {
  return r1;
}