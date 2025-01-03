#ifndef ENCODER_H
#define ENCODER_H

#include "contServo.h"

class Encoder {
  private:
    int encoderPin;
    contServo wiredServo;

    float currentAngle;
    float targetAngle;
    float tolerance;  
    int maxRange;  
    int defaultSpd, servoSpeed;

    float error, previousError;

    // PID
    float kp, ki, kd, integral;
  
  public:
    Encoder(int pin, contServo servo);
    void goToDefaultPos();
    void setTargetAngle(float angle);
    int readAngle();
    float closestAngle(float a, float b);
    float getError();
    float setPID(float error);
    int getServoSpeed();
};

#endif
