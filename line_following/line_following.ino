//
//
// Main control code
//
//

#include <Servo.h>
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

void setup() {
  // Set initial state of QTI sensors
  qti_init();

  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);

  // Set up serial
  Serial.begin(9600);

  Serial.println("Hello");
}

void loop() {
  
  static boolean output[4];
  qti_read(output);
  
  boolean left = output[0];
  boolean center = output[1] || output[2];
  boolean right = output[3];

  char buf[32];
  sprintf(buf, "%d %d %d", left, center, right);
  Serial.println(buf);

  // The robot is on track
  // GO STRAIGHT
  if (!left && center && !right) {
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
  }

  // TURN LEFT
  else
  if (left && !right) {
    servoRight.writeMicroseconds(1400);
    servoLeft.writeMicroseconds(1500);
  }

  // TURN RIGHT
  else
  if (!left && right) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1600);
  }

  // HASH
  else
  if (left && right) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1500);
    delay(1500);
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
    delay(1000);
  }
}
