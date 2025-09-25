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
    int lastRequestedAngle;
    float lastPidOutput;

    Servo servo;
    Encoder& encoder;

  public:
    contServo(int pin, Encoder& encoder);
    void initialize();
    void setSpeed(int speed);
    void stop();

    void setZero();
    void goToAngle(int angle);
    float closestAngle(float target, float current);
    int getDriveDirection() const { return driveDirection; }
    int getCommandedSpeed() const { return totalSpd; }
    float getLastError() const { return error; }
    float getIntegral() const { return integral; }
    float getLastPidOutput() const { return lastPidOutput; }

};

#endif
