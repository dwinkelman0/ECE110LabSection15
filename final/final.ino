/******************************************************************************
 * Code for Chaser 1
 * ECE 110 Lab Section 15
 * Daniel Winkelman, Advaitha Anne, Craig Cronin
 * 
 * BASIC FUNCTIONALITY
 *   1. Initializaion
 *     a. Prints "Chaser 1" to LCD
 *     b. Records a baseline value for QTI sensor for ramps (i.e. what green
 *        looks like)
 *   2. Moves along 5 hashes
 *     a. 3 QTI sensors in front use a four-case line following loop; each QTI
 *        sensor has a capacitor which is charged; an infrared source is shone
 *        downwards; a photoresistor changes the rate of decay of the capacitor
 *        which is reflected in the amount of time the voltage across the
 *        capacitor takes to fall below the threshold of the Arduino digital
 *        inputs
 *     b. If a hash is reached, a QTI sensor detects whether a ramp is present
 *     c. If the ramp is present, the robot pauses and a red LED illuminates;
 *        otherwise, nothing happens; if a ramp is present at the Nth hash, a
 *        1 is shifted into the Nth bit of the data variable; otherwise, a 0 is
 *        shifted in
 *     d. Once the 5th hash has been reached, the robot stops
 *   3. Communication
 *     a. The robot encodes its own message using its ID and data
 *     b. The robot broadcasts its message infinitely once every 1/2 second
 *     c. Whenever the robot is not broadcasting, it listens for any other
 *        message; if it corresponds to a valid robot for which it has not yet
 *        received data, the data is stored in an array
 *     d. This phase ends once all other data has been received
 *   4. Scoring
 *     a. The scores from all robots are aggregated into a total team score
 *     b. The score contributed by this robot and Beater 1 is calculated (i.e.
 *        group score)
 *     c. The team score, the group score, and the number of ramps this robot
 *        detected are printed to the LCD
 *     d. The robot continues to infinitely broadcast its data in the same
 *        manner as step 3b, but no listening occurs
 *        
 */

#include <Servo.h>
#define PIN_SERVO_RIGHT 11
#define PIN_SERVO_LEFT 12
Servo servoRight;
Servo servoLeft;

// Define pins for ramp sensing
#define PIN_RAMP_SENSOR 49 // Sensor pin
#define PIN_RAMP_LED_OUTPUT 6 // Debugging LED pin

// Define pins for QTI sensing
#define PIN_QTI_LEFT 47
#define PIN_QTI_CENTER 51
#define PIN_QTI_RIGHT 52

// Define pins for XBee serial
#define Rx 17 // Transmitting pin
#define Tx 16 // Recieving pin

// Codes defining which robot signal is which
#define ROBOT_ID_CHASER1 0
#define ROBOT_ID_BEATER1 1
#define ROBOT_ID_CHASER2 2
#define ROBOT_ID_BEATER2 3
#define ROBOT_ID_SEEKER  4

// Code for this robot
#define MY_ROBOT_ID ROBOT_ID_CHASER1

// Comment out to disable printing RC times to serial
//#define DEBUG_RC_TIME

/**
 * Compare function for QTI sensing of ramps
 *  e.g. rc_time(PIN_NUMBER, comp_func_ramps);
 */
long comp_func_ramps(long duration) {
  static long threshold = 0;
  if (threshold == 0) {
    threshold = duration;
  }
  return duration < (threshold * 0.75);
}

/**
 * Compare function for QTI sensing of lines
 *  e.g. rc_time(PIN_NUMBER, comp_func_lines);
 */
long comp_func_lines(long duration) {
  const static long WHITE_BLACK_THRESHOLD = 80;
  return duration > WHITE_BLACK_THRESHOLD;
}

/**
 * Record RC decay time on a pin
 * Return result of a specific comparison function
 */
long rc_time(int pin, long comp_func(long)) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);
  pinMode(pin, INPUT);
  digitalWrite(pin, LOW);

  long duration = 0;
  while(digitalRead(pin)) duration++;

  #ifdef DEBUG_RC_TIME
  static char buf[32];
  sprintf(buf, "%d: %d", pin, duration);
  Serial.println(buf);
  #endif

  return comp_func(duration);
}

void setup() {

  // Set up servos
  servoRight.attach(PIN_SERVO_RIGHT);
  servoLeft.attach(PIN_SERVO_LEFT);

  // Set up serials
  Serial.begin(9600); // set up keyboard serial
  Serial2.begin(9600); // set up XBee serial
  Serial3.begin(9600); // set up LCD serial

  // Initialize ramp sensor threshold
  rc_time(PIN_RAMP_SENSOR, comp_func_ramps);

  // Set up diagnostic LED
  pinMode(PIN_RAMP_LED_OUTPUT, OUTPUT);
  
  delay(500);

  // Sanity check for LCD
  Serial3.write(12);
  Serial3.println("Chaser 1");

  // Flush XBee input/output
  Serial2.flush();
  while (Serial2.available()) Serial2.read();
}

void loop() {

  boolean left = rc_time(PIN_QTI_LEFT, comp_func_lines);
  boolean center = rc_time(PIN_QTI_CENTER, comp_func_lines);
  boolean right = rc_time(PIN_QTI_RIGHT, comp_func_lines);

  char buf[32];

  // The number of hashes the robot has to go to has been met
  static int n_hashes = 0;

  // Aggregated data about ramps being at each hash
  // 0 = no ramp, 1 = ramp
  static int data = 0;
  
  // The robot is on track
  // GO STRAIGHT
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
    sprintf(buf, "Hash number %d", n_hashes);
    Serial.println(buf);
    
    servoRight.writeMicroseconds(1495); // not quite even with 1500/1500
    servoLeft.writeMicroseconds(1500);

    delay(500);

    // Sensing
    boolean is_ramp = rc_time(PIN_RAMP_SENSOR, comp_func_ramps);
    data |= is_ramp << n_hashes;
    if (is_ramp) {
      digitalWrite(PIN_RAMP_LED_OUTPUT, HIGH);
      delay(2000);
      digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);
    }

    n_hashes++;

    // If it is not the last hash, move forward some to avoid
    // double-sensing
    if (n_hashes < 5) {
      servoRight.writeMicroseconds(1440);
      servoLeft.writeMicroseconds(1560);
      delay(300);
    }

    // If it is the last hash, broadcast and receive scores
    // This is an infinite loop; nothing else happens
    else {
      communicate_score(MY_ROBOT_ID, data);
    }
  }
}
