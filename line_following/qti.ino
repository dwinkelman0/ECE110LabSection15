//
//
// Code for reading QTI sensors
//
//

// RIGHT 52
//       53
// LEFT  47

const int N_PINS_QTI = 3;
const int PINS_QTI[N_PINS_QTI] = { 47, 53, 52 };


#define WHITE_BLACK_CUTOFF 300

// Set all QTI pins to input or output
void qti_set_io(int io_state) {
  for (int pin_i = 0; pin_i < N_PINS_QTI; pin_i++) {
    pinMode(PINS_QTI[pin_i], io_state);
  }
}

// Set all QTI pins to a certain digital state
void qti_set_state(int state) {
  for (int pin_i = 0; pin_i < N_PINS_QTI; pin_i++) {
    digitalWrite(PINS_QTI[pin_i], state);
  }
}

// Return analog values indicating how long each sensor needs to decay as
// integers in an array
void qti_read_analog(int * output) {
  for (int pin_i = 0; pin_i < N_PINS_QTI; pin_i++) {
    output[pin_i] = 0;
  }
  
  boolean any = 1;
  while (any) {
    any = 0;
    for (int pin_i = 0; pin_i < N_PINS_QTI; pin_i++) {
      int res = digitalRead(PINS_QTI[pin_i]) ? 1 : 0;
      output[pin_i] += res;
      any |= res;
    }
  }
  
  char buf[32];
  sprintf(buf, "%d %d %d", output[0], output[1], output[2]);
  Serial.println(buf);
}

// Return an integer where each bit represents the state of the QTI sensor array
// LSB corresponds to the QTI sensor with the lowest pin number
void qti_read_digital(boolean * output) {
  static int data[N_PINS_QTI];
  qti_read_analog(data);
  for (int pin_i = 0; pin_i < N_PINS_QTI; pin_i++) {
    output[pin_i] = data[pin_i] > WHITE_BLACK_CUTOFF;
  }
}

// Initialize QTI sensors
void qti_init() {
  qti_set_io(INPUT);
  qti_set_state(LOW);
}

// Execute the entire process of charging, discharging, and measuring sensors
void qti_read(boolean * output) {
  // Charge the QTI sensors
  qti_set_io(OUTPUT);
  qti_set_state(HIGH);
  delay(1);

  // Read the QTI sensors
  qti_set_io(INPUT);
  qti_set_state(LOW);
  qti_read_digital(output);
}
