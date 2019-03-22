#include <Servo.h>
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

#define WHITE_BLACK_THRESHOLD 80

// Define pins for ramp sensing
#define PIN_RAMP_SENSOR 49

// Define pins for ramp sensing output
#define PIN_RAMP_LED_OUTPUT 6

// QTI thresholds
#define RAMP_THRESHOLD 600

// Define pins for XBee serial
#define Rx 17 //define transmitting pin
#define Tx 16 //define recieving pin

// Define pins for button and LED inputs/outputs
#define BUTTON_INPUT 10
#define SEND_LED_OUTPUT 5
#define RECEIVE_LED_OUTPUT 7

long RCTime(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);

  long duration = 0;
  while(digitalRead(pin)) duration++;

  return duration > WHITE_BLACK_THRESHOLD;
}

// Function for measuring a QTI input
long rc_time(int pin) {
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

  static long threshold = 0;
  if (threshold == 0) {
    threshold = duration;
  }
  return duration < (threshold * 0.75);
}

void setup() {

  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);

  pinMode(PIN_RAMP_LED_OUTPUT, OUTPUT);
  digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);

  // Set up serials
  Serial.begin(9600); // set up keyboard serial
  Serial2.begin(9600); // set up XBee serial

  // Initialize ramp sensor threshold
  rc_time(PIN_RAMP_SENSOR);

  // Set up communication pins
  pinMode (SEND_LED_OUTPUT, OUTPUT); // set up LED indicating signal sent
  pinMode (RECEIVE_LED_OUTPUT, OUTPUT); // set up LED indicating signal received

  delay(500);
}

void loop() {

  boolean left = RCTime(47);
  boolean center = RCTime(51);
  boolean right = RCTime(52);

  char buf[32];

  // The number of hashes the robot has to go to has been met
  static int n_hashes = 0;
  if (n_hashes >= 5) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1500);
    return;
  }
  
  // The robot is on track
  // GO STRAIGHT
  else
  if (!left && center && !right) {
    servoRight.writeMicroseconds(1435); // correct for right-drifting
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
    n_hashes++;
    sprintf(buf, "Hash number %d", n_hashes);
    Serial.println(buf);
    
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1500);

    delay(500);

    // Sensing
    boolean is_ramp = rc_time(PIN_RAMP_SENSOR);
    if (is_ramp) {
    
      // Send signal
      const char outgoing = '5';
      Serial2.print(outgoing);
  
      // Print signal to serial
      sprintf(buf, "Broadcast signal %c", outgoing);
      Serial.println(buf);
      
      digitalWrite(PIN_RAMP_LED_OUTPUT, HIGH);
      delay(2000);
      digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);
    }
    else {
      
      // Send signal
      const char outgoing = '6';
      Serial2.print(outgoing);

      // Print signal to serial
      sprintf(buf, "Broadcast signal %c", outgoing);
      Serial.println(buf);
    }
    
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
    delay(300);
  }
}
