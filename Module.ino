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
  Serial.println("Starting...");
}

static unsigned long nextTickUs = 0;
static const unsigned long CONTROL_PERIOD_US = 1000; // 1 kHz loop
static String serialBuf;

void loop() {
  // Non-blocking serial parse for combined LX&LY
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      if (serialBuf.length() > 0) {
        commands(serialBuf);
        serialBuf = "";
      }
    } else if (c != '\r') {
      serialBuf += c;
    }
  }

  unsigned long nowUs = micros();
  if ((long)(nowUs - nextTickUs) >= 0) {
    nextTickUs = nowUs + CONTROL_PERIOD_US;
    servoA.goToAngle(targetAngle);
  }
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
