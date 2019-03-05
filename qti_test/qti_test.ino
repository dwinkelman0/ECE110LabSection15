#define PIN_QTI_MIN 5
#define PIN_QTI_MAX 8

#define WHITE_BLACK_CUTOFF 500

// Set all QTI pins to input or output
void set_qti_io(int io_state) {
  for (int pin = PIN_QTI_MIN; pin <= PIN_QTI_MAX; pin++) {
    pinMode(pin, io_state);
  }
}

// Set all QTI pins to a certain digital state
void set_qti_state(int state) {
  for (int pin = PIN_QTI_MIN; pin <= PIN_QTI_MAX; pin++) {
    digitalWrite(pin, state);
  }
}

// Return analog values indicating how long each sensor needs to decay as
// integers in an array
void read_qti_analog(int * output) {
  for (int pin = PIN_QTI_MIN, index = 0; pin <= PIN_QTI_MAX; pin++, index++) {
    output[index] = 0;
  }
  
  boolean any = 1;
  while (any) {
    any = 0;
    for (int pin = PIN_QTI_MIN, index = 0; pin <= PIN_QTI_MAX; pin++, index++) {
      int res = digitalRead(pin) ? 1 : 0;
      output[index] += res;
      any |= res;
    }
  }
}

// Return an integer where each bit represents the state of the QTI sensor array
// LSB corresponds to the QTI sensor with the lowest pin number
int read_qti_digital() {
  int output = 0;
  static int data[PIN_QTI_MAX - PIN_QTI_MIN + 1];
  read_qti_analog(data);
  for (int pin = PIN_QTI_MIN, index = 0; pin <= PIN_QTI_MAX; pin++, index++) {
    output |= ((data[index] > WHITE_BLACK_CUTOFF) ? 1 : 0) << index;
  }
  return output;
}

void setup() {
  // Set initial state of QTI sensors
  set_qti_io(INPUT);
  set_qti_state(LOW);

  // Set up serial
  Serial.begin(9600);
}

void loop() {
  // Charge the QTI sensors
  set_qti_io(OUTPUT);
  set_qti_state(HIGH);
  delay(1);

  // Read the QTI sensors
  
  set_qti_io(INPUT);
  set_qti_state(LOW);
  int res = read_qti_digital();

  static char buf[32];
  sprintf(buf, "%d", res);
  Serial.println(buf);

  delay(100);
}
