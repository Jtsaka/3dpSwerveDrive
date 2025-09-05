  #include "contServo.h"
  #include "encoder.h"
  #include "spark.h"

  contServo servoA(5); //module 3
  Spark sparkA(9); //module 3

  Encoder encoderA(A3, servoA, sparkA); //encoder to module 1
  
  int Vx = 0;    //X translation from joystick
  int Vy = 0;    //Y translation from joystick
  int angle = 0;
  int deadzone = 1000;
  int defaultSpd = 1500;
  int speedFactor = 300;

  void setup() {
    
      Serial.begin(500000); //baud rate

      sparkA.initialize();
      encoderA.goToDefaultPos();
      encoderA.goToAngle();
      servoA.initialize();

  }

  void loop() {
    if (Serial.available() > 0) {
      String data = Serial.readStringUntil('\n');
      commands(data);
          
      encoderA.setTargetAngle(angle);
      
      delay(100);  //Small delay to prevent overwhelming the servo

    }

    encoderA.goToAngle();

  }

//UP/DOWN = 0 degrees, LEFT/RIGHT = 90 degrees, DIAGONAL CALCULATED THROUGH ARCTAN

  void updateAngle(int x, int y){
    if(abs(x) <= deadzone && abs(y) <= deadzone){
      sparkA.setSpeed(defaultSpd);
    }
    else if(abs(x) <= deadzone && abs(y) > deadzone){
      if(y > 0){ //angle = (y > 0) ? 0 : 180;
        angle = 0;
        sparkA.setSpeed(defaultSpd + speedFactor);
      }
      else{
        angle = 180;
        sparkA.setSpeed(defaultSpd - speedFactor);
      }
    }
    else if(abs(y) <= deadzone && abs(x) > deadzone){
      if(x > 0){ //angle = (x > 0) ? 90 : 270;
        angle = 90;
        sparkA.setSpeed(defaultSpd + speedFactor);
      }
      else{
        angle = 270;
        sparkA.setSpeed(defaultSpd - speedFactor);
      }
    }
    else{
      if(abs(y) >= deadzone && abs(x) >= deadzone){
        int angle_radians = atan2(y, x);
        angle = angle_radians * (180 / M_PI);
      }
    }
  }

  void commands(String data){
    //Drive joystick
    if (data.startsWith("stickLX&LY:")) { 
        String value = data.substring(11);  //Get the value after "stickLXLY:"
        int commaSep = value.indexOf(",");

        String vxStr = value.substring(0, commaSep);
        Vx = vxStr.toInt();
        String vyStr = value.substring(commaSep + 1);
        Vy = vyStr.toInt();

        updateAngle(Vx, Vy);

        Serial.println("Left joystick X value: " + String(Vx) + " |   Left joystick Y value: " + String(Vy) + " | Current Angle: " + String(encoderA.readAngle()) + " | Target angle: " + String(angle) + " | Output power: " + String(encoderA.goToAngle()));

    }
    if (data == "BUTTON_A") {
        Serial.println("Stop");
        sparkA.stopMotor();
    }
    //Steer joystick
    // else if (data.startsWith("stickRX:")) {
    //     String value = data.substring(8);  // Get the value after "stickRY:"
    //     Serial.println("Right joystick X value: " + value);
    //     angle = atoi(value.c_str());
    // }
  }
