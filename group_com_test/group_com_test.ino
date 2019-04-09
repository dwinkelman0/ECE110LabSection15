// Define pins for XBee serial
#define Rx 17 // define transmitting pin
#define Tx 16 // define recieving pin

void setup() {
  // Set up communication
  Serial.begin(9600); // set up keyboard serial
  Serial2.begin(9600); // set up XBee serial
  
  delay(500);

  char messages[] = {
    // Send data
    0 * 32 + 4,
    1 * 32 + 4,
    2 * 32 + 4,
    3 * 32 + 4,
    4 * 32 + 4,

    // Send confirmation
    5 * 32 + 0,
    5 * 32 + 1,
    5 * 32 + 2,
    5 * 32 + 3,
    5 * 32 + 4,

    // Send "done"
    6 * 32 + 0,
    6 * 32 + 1,
    6 * 32 + 2,
    6 * 32 + 3,
    6 * 32 + 4,

    // NULL char
    0
  };

  for (char * m = messages; *m != NULL; m++) {
    Serial2.write(*m);
    static char buf[32];
    sprintf(buf, "Sent %d %d", (*m >> 5) & 7, *m & 31);
    Serial.println(buf);
    delay(3000);
  }
}

void loop() {
  delay(1000);
}
