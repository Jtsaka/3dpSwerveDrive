#include "contServo.h"
#include "Arduino.h" 
#include "math.h"

contServo::contServo(int pin, Encoder& encoder) : servoPin(pin), encoder(encoder), defaultSpd(1500), tolerance(5), servoSpd(defaultSpd), rotateSpd(0), totalSpd(1500), error(0.0), integral(0.0), previousError(0.0), previousTime(0), driveDirection(1){
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

  float kp = 3.5;
  float ki = -1.0;
  float kd = 0.5;

  integral += error * dt;
  float derivative = (error - previousError) / dt;

  int pidOutput = (kp * error) + (ki * integral) + (kd * derivative);

  totalSpd = defaultSpd + pidOutput;

  if (abs(error) <= tolerance) {
    stop();
  } else {
    totalSpd = constrain(totalSpd, 1200, 1800);
    setSpeed(totalSpd);
  }

  previousError = error;
  previousTime = currTime;
}
