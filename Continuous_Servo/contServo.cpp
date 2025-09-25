#include "contServo.h"

#include "Arduino.h"
#include "math.h"

namespace {

float clampf(float value, float minValue, float maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}

float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
  if (inMax - inMin == 0.0f) {
    return outMin;
  }
  float ratio = (x - inMin) / (inMax - inMin);
  return outMin + (ratio * (outMax - outMin));
}

}  // namespace

contServo::contServo(int pin, Encoder& encoder)
  : servoPin(pin),
    encoder(encoder),
    defaultSpd(1500),
    tolerance(5),
    servoSpd(defaultSpd),
    rotateSpd(0),
    totalSpd(1500),
    error(0.0f),
    integral(0.0f),
    previousError(0.0f),
    previousTime(0),
    driveDirection(1),
    lastRequestedAngle(-1),
    lastPidOutput(0.0f) {
}

void contServo::setSpeed(int speed){
  servo.writeMicroseconds(speed);
}

void contServo::initialize(){
  servo.attach(servoPin);
  setSpeed(defaultSpd);
  previousTime = millis();
  integral = 0.0f;
  previousError = 0.0f;
  error = 0.0f;
  lastRequestedAngle = -1;
  lastPidOutput = 0.0f;

}

void contServo::stop(){
  setSpeed(defaultSpd);
  servoSpd = defaultSpd;
  rotateSpd = 0;
  totalSpd = defaultSpd;
  error = 0.0f;
  integral = 0.0f;
  previousError = 0.0f;
  lastRequestedAngle = -1;
  previousTime = millis();
  lastPidOutput = 0.0f;
}

void contServo::setZero(){
  goToAngle(0);
}

float contServo::closestAngle(float target, float currentAngle){
  float diff = target - currentAngle;

  while (diff > 180.0f) {
    diff -= 360.0f;
  }
  while (diff < -180.0f) {
    diff += 360.0f;
  }

  return diff;
}

void contServo::goToAngle(int angle) {
  unsigned long currTime = millis();
  float dt = (currTime - previousTime) / 1000.0f;
  if (dt <= 0.0f) {
    dt = 0.001f;
  } else if (dt > 0.5f) {
    dt = 0.5f;
  }

  int normalizedTarget = angle % 360;
  if (normalizedTarget < 0) {
    normalizedTarget += 360;
  }

  float currentAngle = encoder.readAngle();
  float angleError = closestAngle(static_cast<float>(normalizedTarget), currentAngle);

  driveDirection = +1;
  int effectiveTarget = normalizedTarget;
  if (angleError > 90.0f || angleError < -90.0f) {
    effectiveTarget = (normalizedTarget + 180) % 360;
    angleError = closestAngle(static_cast<float>(effectiveTarget), currentAngle);
    driveDirection = -1;
  }

  if (effectiveTarget != lastRequestedAngle) {
    integral = 0.0f;
    previousError = angleError;
    lastRequestedAngle = effectiveTarget;
  }

  const float kp = 4.0f;
  const float ki = -0.6f;
  const float integralWindow = 70.0f;
  const float integralLimit = 100.0f;

  if (fabs(angleError) < integralWindow) {
    integral += angleError * dt;
    integral = clampf(integral, -integralLimit, integralLimit);
  } else {
    integral = 0.0f;
  }

  float pidOutput = (kp * angleError) + (ki * integral);
  pidOutput = clampf(pidOutput, -450.0f, 450.0f);

  if (fabs(angleError) <= tolerance) {
    setSpeed(defaultSpd);
    error = angleError;
    totalSpd = defaultSpd;
    previousError = angleError;
    lastPidOutput = pidOutput;
    previousTime = currTime;
    return;
  }

  float minimumEffort = mapFloat(fabs(angleError), 0.0f, 180.0f, 70.0f, 420.0f);
  minimumEffort = clampf(minimumEffort, 70.0f, 420.0f);

  float speedOffset = pidOutput;
  if ((speedOffset > 0.0f && angleError < 0.0f) || (speedOffset < 0.0f && angleError > 0.0f)) {
    speedOffset = 0.0f;
  }

  if (fabs(speedOffset) < minimumEffort) {
    speedOffset = (angleError >= 0.0f) ? minimumEffort : -minimumEffort;
  }

  int commandedSpeed = defaultSpd + static_cast<int>(round(speedOffset));
  commandedSpeed = constrain(commandedSpeed, 1000, 2000);

  setSpeed(commandedSpeed);

  error = angleError;
  previousError = angleError;
  totalSpd = commandedSpeed;
  lastPidOutput = pidOutput;
  previousTime = currTime;
}
