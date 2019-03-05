#include <Servo.h>
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

void setup() {
  // put your setup code here, to run once:
  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);
}

void loop() {
  // put your main code here, to run repeatedly:
  servoRight.writeMicroseconds(1400);
  servoLeft.writeMicroseconds(1600);
    
}
