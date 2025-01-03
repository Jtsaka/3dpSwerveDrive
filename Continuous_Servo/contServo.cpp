#include "contServo.h"

contServo::contServo(int pin) : servoPin(pin), defaultSpd(1500){
}

void contServo::initialize(){
  servo.attach(servoPin);
  servo.writeMicroseconds(defaultSpd);
}

int contServo::getDefaultSpd(){
  return defaultSpd;
}

void contServo::setSpeed(int speed){
  servo.writeMicroseconds(speed);
}

void contServo::stop(){
  setSpeed(1500);
}    
