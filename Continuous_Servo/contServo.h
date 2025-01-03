#ifndef CONTSERVO_H
#define CONTSERVO_H

#include <Servo.h>

class contServo { 
  private:
    int servoPin; //signal pin for servo
    int defaultSpd;
    Servo servo;
  public:
    contServo(int pin);
    void initialize();
    int getDefaultSpd();
    void setSpeed(int speed);
    void stop();
};

#endif
