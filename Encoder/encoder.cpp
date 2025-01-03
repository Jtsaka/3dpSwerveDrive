#include <Arduino.h> 
#include "encoder.h"

Encoder::Encoder(int pin, contServo servo) : encoderPin(pin), wiredServo(servo), currentAngle(0.0), targetAngle(0.0), tolerance(3.0), maxRange(2000), defaultSpd(1500), servoSpeed(defaultSpd), error(0.0), previousError(0.0), kp(3.0), ki(0.3), kd(1.5), integral(0.0){
  pinMode(encoderPin, INPUT);
}

void Encoder::goToDefaultPos(){ //set angle to 0, call in main
  setTargetAngle(0.0);
  setPID(getError());
}

void Encoder::setTargetAngle(float angle){ //call in main
  targetAngle = angle;
}

int Encoder::readAngle(){
  float rawValue = analogRead(encoderPin);
  currentAngle = (rawValue / 1023.0) * 360.0; 
  return currentAngle;
}

float Encoder::closestAngle(float a, float b) {      //Calculate circular error 

  float errorCalc = fmod(b - a + 360.0, 360.0);      //error is positive
  if (errorCalc > 180) {
      errorCalc -= 360;                              //Correct direction - counterclockwise
  }
  return errorCalc;
}

float Encoder::getError(){ //call in main within setPID
  error = closestAngle(targetAngle, readAngle());
  return error;
}

float Encoder::setPID(float error){ //call in main
float pVal = kp * error;

integral += error;                                //Track cumulative error
float iVal = ki * integral;

float dVal = kd * (error - previousError);
previousError = error;

float pidVal = pVal + iVal + dVal;

if (abs(error) <= tolerance) {                      //Stop servo if within toleranced angle
      servoSpeed = defaultSpd;                      //Stop the servo
      integral = 0;                                 //Reset cumulataive error
      pidVal = 0;                                   //Reset pid
  } 
else {
    
    if (pidVal > 0) {                             //maxRange constrains the pidVal (prevents erratic values)
        servoSpeed = defaultSpd + constrain(pidVal, 1000, maxRange); 
    } 
    else {
        servoSpeed = defaultSpd - constrain(-pidVal, 1000, maxRange);
    }
}

wiredServo.setSpeed(servoSpeed);     //servo speed
return pidVal;

}

int Encoder::getServoSpeed(){
  return servoSpeed;
}
