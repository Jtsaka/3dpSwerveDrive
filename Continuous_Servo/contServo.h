#ifndef CONTSERVO_H
#define CONTSERVO_H

#include <Servo.h>
#include "encoder.h"

class contServo { 
  private:
    int servoPin; 
    int defaultSpd;
    int tolerance;
    int servoSpd;
    int rotateSpd;
    int totalSpd;
    float error;
    float integral;
    float previousError;
    unsigned long previousTime;
    int driveDirection;
    float maxOmegaDegPerSec;
    float maxAlphaDegPerSec2;
    float omegaCmd;

    Servo servo;
    Encoder& encoder;

  public:
    contServo(int pin, Encoder& encoder);
    void initialize();
    void setSpeed(int speed);
    void stop();

    void setZero();
    void goToAngle(int angle);
    int closestAngle(int a, int b);
    void configureLimits(float maxOmegaDegSec, float maxAlphaDegSec2);
    void setPIDGains(float kp, float ki, float kd);

  private:
    float kp;
    float ki;
    float kd;

};

#endif
