#include <Arduino.h> 
#include "encoder.h"

Encoder::Encoder(int pin) : encoderPin(pin), currentAngle(0), filteredAngleDeg(0.0f), smoothingAlpha(0.2f), isInitialized(false){
}

int Encoder::readAngle(){
  int rawValue = analogRead(encoderPin);
  float measuredDeg = (rawValue / 1023.0f) * 360.0f; // 0..360

  if (!isInitialized) {
    filteredAngleDeg = measuredDeg;
    isInitialized = true;
  } else {
    // Wrap-aware exponential smoothing to avoid 0/360 jump
    float delta = measuredDeg - filteredAngleDeg;
    if (delta > 180.0f) delta -= 360.0f;
    if (delta < -180.0f) delta += 360.0f;
    filteredAngleDeg = filteredAngleDeg + smoothingAlpha * delta;
    if (filteredAngleDeg < 0.0f) filteredAngleDeg += 360.0f;
    if (filteredAngleDeg >= 360.0f) filteredAngleDeg -= 360.0f;
  }

  currentAngle = (int)round(filteredAngleDeg);
  return currentAngle;
}

void Encoder::setSmoothingAlpha(float alpha) {
  if (alpha < 0.0f) alpha = 0.0f;
  if (alpha > 1.0f) alpha = 1.0f;
  smoothingAlpha = alpha;
}
