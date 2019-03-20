// Define pins for ramp sensing
#define PIN_RAMP_SENSOR 49

// Define pins for ramp sensing output
#define PIN_RAMP_LED_OUTPUT 6

// QTI thresholds
#define RAMP_THRESHOLD 1100

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
  pinMode(PIN_RAMP_LED_OUTPUT, OUTPUT);
  digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);
  
  Serial.begin(9600);
}

void loop() {

  // Check for ramp
  boolean is_ramp = !rc_time(PIN_RAMP_SENSOR, RAMP_THRESHOLD);
  if (is_ramp) {
    digitalWrite(PIN_RAMP_LED_OUTPUT, HIGH);
    delay(2000);
    digitalWrite(PIN_RAMP_LED_OUTPUT, LOW);
  }
  else {
    delay(2000);
  }
}
