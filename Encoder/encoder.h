#ifndef ENCODER_H
#define ENCODER_H

#include "spark.h"

class Encoder {
  private:
    int encoderPin;
    
    int currentAngle;
    float filteredAngleDeg;
    float smoothingAlpha; // 0..1, fraction of new error applied each update
    bool isInitialized;

    float wrapDegrees(float degrees) {
      while (degrees >= 180.0f) degrees -= 360.0f;
      while (degrees < -180.0f) degrees += 360.0f;
      return degrees;
    }

  public:
    Encoder(int pin);
    int readAngle();
    void setSmoothingAlpha(float alpha);

};

#endif
