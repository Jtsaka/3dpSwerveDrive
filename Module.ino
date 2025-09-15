#include "contServo.h"
#include "encoder.h"

  Encoder encoderA(A3);
  contServo servoA(5, encoderA); //module 3
  // Spark sparkA(9); //module 3
 
  int Vx = 0;    
  int Vy = 0;    
  const int deadzone = 3000;
  // int defaultSpd = 1500;
  int motorSpd = 1500; //ignore
  int targetAngle = 0;
  unsigned long lastPrint = 0;

void setup() {
  Serial.begin(500000); //baud rate
  servoA.initialize();
  // sparkA.initialize();
  Serial.println("Starting...");

}

void loop() {
  if (Serial.available() > 0) {
      String data = Serial.readStringUntil('\n');
      commands(data);      
  }
  servoA.goToAngle(targetAngle);

  // Serial.println("Left joystick X value: " + String(Vx) + " |   Left joystick Y value: " + String(Vy) + " | Curr Angle: " + String(encoderA.readAngle()) + " | Target angle: " + String(targetAngle));
}

void updateAngle(int x, int y){
    if (abs(x) < deadzone && abs(y) < deadzone){
      servoA.stop();
      return;
    }

    float angle_radians = atan2(x, y); //if I do atan2(y,x), then 0 on x axis (cartesian), swap to x,y to rotate 90 deg
    float angle_degrees = angle_radians * (180.0 / PI);  
    if (angle_degrees < 0) {
      angle_degrees += 360.0;  //normalize to [0, 360)
    }
    
    targetAngle = (int)round(angle_degrees); 
    
    // int currAngle = encoderA.readAngle();  
    // int difference = (targetAngle - currAngle + 540) % 360 - 180;
    
    // int magnitude = sqrt((x * x) + (y * y));

    // if(abs(difference) > 90){
    //   motorSpd = defaultSpd - (0.01 * magnitude);
    // }

    // else{
    //   motorSpd = defaultSpd + (0.01 * magnitude);
    // }
    
    // sparkA.setSpeed(motorSpd);
    // else{
    //    sparkA.setSpeed(defaultSpd);
    // }
  }

void commands(String data){
    //Drive joystick
    if (data.startsWith("stickLX&LY:")) { 
        String value = data.substring(11);  //get the value after "stickLXLY:"
        int commaSep = value.indexOf(",");

        String vxStr = value.substring(0, commaSep);
        Vx = vxStr.toInt();
        String vyStr = value.substring(commaSep + 1);
        Vy = vyStr.toInt();

        updateAngle(Vx, Vy);

    }
    if (data == "BUTTON_A") {
        Serial.println("Stop");
        servoA.stop();
    }
    //Steer joystick
    // else if (data.startsWith("stickRX:")) {
    //     String value = data.substring(8);  // Get the value after "stickRY:"
    //     Serial.println("Right joystick X value: " + value);
    //     angle = atoi(value.c_str());
    // }
  }
