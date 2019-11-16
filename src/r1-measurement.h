#ifndef R1_MEASUREMENT_H_
#define R1_MEASUREMENT_H_

#define VIN A0
#define VOUT A1
#define CONTROL 10

class R1Measurement {
  private:
    float _r1;
    float _r2 = 1000.0;
    float _vref = 5.0;

  public:
    R1Measurement();

    void update();
    float get();
};

#endif