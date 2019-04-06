
// Codes defining which robot signal is which
#define ROBOT_ID_CHASER1 0
#define ROBOT_ID_BEATER1 1
#define ROBOT_ID_CHASER2 2
#define ROBOT_ID_BEATER2 3
#define ROBOT_ID_SEEKER  4

// TODO: change this for your robot
#define MY_ROBOT_ID ROBOT_ID_CHASER1

// Define pins for XBee serial
#define Rx 17 // define transmitting pin
#define Tx 16 // define recieving pin

// Needed for dummy implementation
#include <stdlib.h>

int hash() {
  // Dummy implementation for sensing a hash mark
  delay(1000);
  return 1;
}

int sense() {
  // Dummy implementation which returns a random result
  static bool rand_init = false;
  if (!rand_init) {
    srand(MY_ROBOT_ID);
    rand_init = true;
  }
  return rand() & 1;
}

void setup() {
  // Set up communication
  Serial.begin(9600); // set up keyboard serial
  Serial2.begin(9600); // set up XBee serial
  
  delay(500);
}

void loop() {
  // Counts the number of hashes the robot has encountered
  static int n_hashes = 0;

  // Records the data of the robot (e.g. is there a ramp at a hash)
  // For Seeker, this is 1 if the snitch is found, 0 if the snitch is not found
  // For other robots, this is 5 bits representing the state of each hash (see below)
  static int data = 0;

  // Code that runs if a hash is detected
  // Note: this does not apply to Seeker
  if (hash) {
    // TODO: replace this function with your robot's sensory task
    // Chaser 1: must return true (1) if ramp, false (0) if no ramp
    // Beater 1: must return true (1) if RFID tag, false (0) if no RFID tag
    // Chaser 2: must return true (1) if quaffle, false (0) if no quaffle
    // Beater 2: must return true (1) if green, false (0) if red
    int sense_result = sense();

    // Stores data, records the presence of a hash
    data |= (sense_result & 1) << n_hashes;
    n_hashes++;
  }

  // Note: this only applies to Seeker
  if (MY_ROBOT_ID == ROBOT_ID_SEEKER) {
    if (sense()) {
      data = 1;
    }
    else {
      data = 0;
    }
  }

  if (n_hashes >= 5) {
    // Stop moving, start broadcasting and waiting for other robot's scores
    // Note: this code is implemented for the Seeker when the snitch is found or not found
    
    // TODO: implement code for stopping your robot

    // Broadcast this robot's information and get other robots' scores
    int team_score = score(MY_ROBOT_ID, data);

    // Print team score to LCD
    // TODO: implement this
  }
}
