#include <Arduino.h> 
#include "encoder.h"
#include "spark.h"


Encoder::Encoder(int pin, contServo& servo, Spark& spark) : encoderPin(pin), wiredSpark(spark), wiredServo(servo), currentAngle(0.0), targetAngle(0.0), tolerance(5.0), maxRange(300), defaultSpd(1500), servoSpeed(defaultSpd), error(0.0){
  pinMode(encoderPin, INPUT);
  //targetAngle = readAngle();

}

void Encoder::goToDefaultPos(){ //set angle to 0, call in main
  setTargetAngle(0);
  return;
}

void Encoder::setTargetAngle(float angle){ 
  if (targetAngle != angle) {
        targetAngle = angle;
  }
  return; 
} 

float Encoder::readAngle(){
  int rawValue = analogRead(encoderPin);
  currentAngle = (rawValue / 1023.0) * 360.0; 
  return currentAngle;
}

float Encoder::closestAngle(float target, float currentAngle) {      //Calculate circular error 
  float errorCalc = fmod(target - currentAngle + 360.0, 360.0);      //error is positive

  //maybe do a line of code where if errorCalc is 360, then make it 0? Only if oscillation occurs at 360-0
  if (errorCalc > 180) {
      errorCalc -= 180;                              //Correct direction if greater than 180, counterclockwise
  }
  return errorCalc;
}

float Encoder::goToAngle(){
  error = closestAngle(targetAngle, readAngle());
  if (abs(error) <= tolerance) {
    //Serial.println("Stopped"); //prints infinite stops currently
    wiredServo.stop();
    return;
  }
  float speedFactor = abs(error) / 180.0 * maxRange;

  // if(targetAngle >= 0 && targetAngle < 180){
  //   wiredSpark.setSpeed(1800);
  // }
  // else{
  //   wiredSpark.setSpeed(1200);
  // }
  
  if (error > tolerance) {
    servoSpeed = defaultSpd + constrain(speedFactor, 0, maxRange);
  } 
  else {
    servoSpeed = defaultSpd - constrain(speedFactor, 0, maxRange);
  }
  wiredServo.setSpeed(servoSpeed);

  return servoSpeed;
}
