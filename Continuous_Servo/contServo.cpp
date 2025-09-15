#include "contServo.h"
#include "Arduino.h" 
#include "math.h"

contServo::contServo(int pin, Encoder& encoder) : servoPin(pin), encoder(encoder), defaultSpd(1500), tolerance(5), servoSpd(defaultSpd), rotateSpd(0), totalSpd(1500), error(0.0), integral(0.0), previousError(0.0), previousTime(0), driveDirection(1), maxOmegaDegPerSec(500.0f), maxAlphaDegPerSec2(4000.0f), omegaCmd(0.0f), kp(3.5f), ki(0.0f), kd(0.5f){
}

void contServo::setSpeed(int speed){
  servo.writeMicroseconds(speed);
}

void contServo::initialize(){
  servo.attach(servoPin);
  setSpeed(defaultSpd); 
  previousTime = millis();

}

void contServo::stop(){
  setSpeed(1500);
}

void contServo::setZero(){
  goToAngle(0);
}

int contServo::closestAngle(int target, int currentAngle){
  return fmod((target - currentAngle + 540.0), 360.0) - 180.0;
}
    
void contServo::goToAngle(int angle) {
  unsigned long currTime = millis();
  float dt = (currTime - previousTime) / 1000.0;
  if (dt <= 0) dt = 0.001;

  error = closestAngle(angle, encoder.readAngle());

  if (error > 90) {
    angle = (angle + 180) % 360;
    error = closestAngle(angle, encoder.readAngle());
    driveDirection = -1; //backwards
  }
  else if (error < -90) {
    angle = (angle + 180) % 360;
    error = closestAngle(angle, encoder.readAngle());
    driveDirection = -1; //backwards
  }
  else {
    driveDirection = +1; //forward
  }

  // PID with positive KI (integrator winds toward reducing steady-state error)
  integral += error * dt;
  float derivative = (error - previousError) / dt;
  float rawOmega = (kp * error) + (ki * integral) + (kd * derivative); // deg/s equivalent

  // Velocity and acceleration limiting for smoothness
  float maxDeltaOmega = maxAlphaDegPerSec2 * dt;
  if (rawOmega >  maxOmegaDegPerSec) rawOmega =  maxOmegaDegPerSec;
  if (rawOmega < -maxOmegaDegPerSec) rawOmega = -maxOmegaDegPerSec;
  float delta = rawOmega - omegaCmd;
  if (delta >  maxDeltaOmega) delta =  maxDeltaOmega;
  if (delta < -maxDeltaOmega) delta = -maxDeltaOmega;
  omegaCmd += delta;

  // Map omegaCmd (deg/s) to servo microseconds around 1500
  const float usPerDegPerSec = 1.0f; // tune experimentally
  totalSpd = (int)(defaultSpd + omegaCmd * usPerDegPerSec);

  if (abs(error) <= tolerance) {
    stop();
    omegaCmd = 0.0f;
  } else {
    totalSpd = constrain(totalSpd, 1200, 1800);
    setSpeed(totalSpd);
  }

  previousError = error;
  previousTime = currTime;
}

void contServo::configureLimits(float maxOmegaDegSec, float maxAlphaDegSec2) {
  if (maxOmegaDegSec < 10.0f) maxOmegaDegSec = 10.0f;
  if (maxAlphaDegSec2 < 100.0f) maxAlphaDegSec2 = 100.0f;
  this->maxOmegaDegPerSec = maxOmegaDegSec;
  this->maxAlphaDegPerSec2 = maxAlphaDegSec2;
}

void contServo::setPIDGains(float p, float i, float d) {
  this->kp = p;
  this->ki = i;
  this->kd = d;
}
