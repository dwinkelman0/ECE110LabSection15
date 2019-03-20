#include <Servo.h>

// Define pins for ramp sensing
#define PIN_RAMP_SENSOR 49

// Define pins for ramp sensing output
#define PIN_RAMP_LED_OUTPUT 6

// Character unique to robot
#define UNIQUE_ROBOT_CODE '5'

// Define pins for XBee serial
#define Rx 17 // define transmitting pin
#define Tx 16 // define recieving pin

// Define pins for button and LED inputs/outputs
#define BUTTON_INPUT 10
#define SEND_LED_OUTPUT 5
#define RECEIVE_LED_OUTPUT 7

// Define pins for servos
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

// QTI thresholds
#define WHITE_BLACK_THRESHOLD 80
#define RAMP_THRESHOLD 100

// Function for measuring a QTI input
long rc_time(int pin, long threshold) {
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

  return duration > threshold;
}

void setup() {

  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);

  // Set up serials
  Serial.begin(9600);  // Computer
  Serial2.begin(9600); // XBee

  // Communication buttons
  pinMode (BUTTON_INPUT, INPUT); // set up button pin
  pinMode (SEND_LED_OUTPUT, OUTPUT); // set up LED indicating signal sent
  pinMode (RECEIVE_LED_OUTPUT, OUTPUT); // set up LED indicating signal received
  
  delay(500);
}

void communication() {

  static char buf[32];
  
  // If button pressed, send signal
  if (digitalRead(BUTTON_INPUT)) {
    
    // Send signal
    char outgoing = UNIQUE_ROBOT_CODE;
    Serial2.print(outgoing);

    // Print signal to serial
    sprintf(buf, "Broadcast signal %c", outgoing);
    Serial.println(buf);

    // Blink send indicator LED
    digitalWrite(SEND_LED_OUTPUT, HIGH);
    delay(1500);
    digitalWrite(SEND_LED_OUTPUT, LOW);
  }

  // If signal received from XBee serial
  if (Serial2.available()){

    // Receive signal
    char incoming = Serial2.read();

    // Check that signal is not coming from this robot
    if (incoming != UNIQUE_ROBOT_CODE) {

      // Print signal to serial
      sprintf(buf, "Received signal %c", incoming);
      Serial.println(buf);
      
      // Blink receive indicator LED
      digitalWrite(RECEIVE_LED_OUTPUT, HIGH);
      delay(1500);
      digitalWrite(RECEIVE_LED_OUTPUT, LOW);
    }
  }
}

void line_following () {
  boolean left = rc_time(47, WHITE_BLACK_THRESHOLD);
  boolean center = rc_time(51, WHITE_BLACK_THRESHOLD);
  boolean right = rc_time(52, WHITE_BLACK_THRESHOLD);

  static char buf[32];
  sprintf(buf, "%d %d %d", left, center, right);
  Serial.println(buf);

  // The robot is on track
  // GO STRAIGHT
  if (!left && center && !right) {
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
  }

  // TURN LEFT
  else if (left && !right) {
    servoRight.writeMicroseconds(1350);
    servoLeft.writeMicroseconds(1400);
  }

  // TURN RIGHT
  else if (!left && right) {
    servoRight.writeMicroseconds(1600);
    servoLeft.writeMicroseconds(1650);
  }

  // HASH
  else if (left && right) {
    servoRight.writeMicroseconds(1500);
    servoLeft.writeMicroseconds(1500);

    // Check for ramp
    boolean is_ramp = rc_time(PIN_RAMP_SENSOR, RAMP_THRESHOLD);
    if (is_ramp) {
      digitalWrite(PIN_RAMP_LED_OUTPUT, HIGH);
      delay(2000);
      digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);
    }
    else {
      delay(2000);
    }
    
    servoRight.writeMicroseconds(1440);
    servoLeft.writeMicroseconds(1560);
    delay(1000);
  }
}

void loop() {

  // Memory for serial printing string formatting
  char buf[32];

  communication();
  
  line_following();
}
