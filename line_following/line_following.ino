//
//
// Main control code
//
// Help with conditional and servo code from Emily Barre
//

#include <Servo.h>
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

#define WHITE_BLACK_THRESHOLD 80

long RCTime(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);
  
  long duration = 0;
  while(digitalRead(pin)) duration++;

  char buf[32];
  sprintf(buf, "%d: %d", pin, duration);
  Serial.println(buf);
  
  return duration > WHITE_BLACK_THRESHOLD;
}

void setup() {
  // Set initial state of QTI sensors
  //qti_init();

  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);

  // Set up serial
  Serial.begin(9600);

  Serial.println("Hello");
}

void loop() {

  /*
  static boolean output[3];
  qti_read(output);
  
  boolean left = output[0];
  boolean center = output[1];
  boolean right = output[2];
  */

/*
  long leftL = RCTime(47); long centerL = RCTime(51); long rightL = RCTime(52);

  char buf[32];
  sprintf(buf, "%d %d %d", leftL, centerL, rightL);
  Serial.println(buf);
  */
/*
  boolean left = leftL > WHITE_BLACK_THRESHOLD;
  boolean center = centerL > WHITE_BLACK_THRESHOLD;
  boolean right = rightL > WHITE_BLACK_THRESHOLD;
  */

  boolean left = RCTime(47);
  boolean center = RCTime(51);
  boolean right = RCTime(52);

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
    servoRight.writeMicroseconds(1350);
    servoLeft.writeMicroseconds(1500);
  }

  // TURN RIGHT
  else
  if (!left && right) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1650);
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

  // STOP
  else
  if (!left && !center && !right) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1500);
    delay(1000);
  }
}
