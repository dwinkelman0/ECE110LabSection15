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
  boolean center_left = output[1];
  boolean center_right = output[2];
  boolean right = output[3];

  char buf[32];
  sprintf(buf, "%d %d %d %d", left, center_left, center_right, right);
  Serial.println(buf);

  // The robot is on track
  // GO STRAIGHT
  if (!left &&  center_left &&  center_right && !right) {
    Serial.println("Forward");
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
    delay(50);
  }

  // The robot is veering to the right of the path
  // TURN SLIGHTLY LEFT
  else
  if (center_left && !center_right) {
    Serial.println("Veer left");
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1520);
    delay(50);
  }

  // The robot is veering to the left of the path
  // TURN SLIGHTLY RIGHT
  else
  if (!center_left &&  center_right) {
    Serial.println("Veer right");
    servoRight.writeMicroseconds(1480);
    servoLeft.writeMicroseconds(1560);
    delay(50);
  }

  else
  if (left) {
    servoRight.writeMicroseconds(1350);
    servoLeft.writeMicroseconds(1470);
    delay(50);
  }

  else
  if (right) {
    servoRight.writeMicroseconds(1530);
    servoLeft.writeMicroseconds(1650);
    delay(50);
  }

  else {
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
    delay(50);
  }

  /*
  // The robot is at a right corner
  // TURN RIGHT
  else
  if (!left &&  center_left &&  center_right &&  right) {
    servoRight.writeMicroseconds(
  }
  */
}
