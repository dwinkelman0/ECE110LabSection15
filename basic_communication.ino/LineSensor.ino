void setup()                                 // Built in initialization block
{
  servoRight.attach(1);                     // Attach right signal to pin 1

  servoLeft.attach(2);                       // Attach left signal to pin 2

  pinMode(7, INPUT);                         // Set end whisker pin to input
  pinMode(5, INPUT);                         // Set end whisker pin to input  
  pinMode(6, INPUT);                         // Set middle whisker pin to input
  

}  
 
void loop()                                  
{
  byte middle = digitalRead(6)
  byte right = digitalRead(7)
  byte left = digitalRead(5)

  if (middle == 1) {
  servoRight.writeMicroseconds(1700);        // Right wheel clockwise
  servoLeft.writeMicroseconds(1300);   
  }

  if (right == 1) {
  servoRight.writeMicroseconds(1000);        // Right wheel clockwise
  servoLeft.writeMicroseconds(1700);   
  }

 if (left == 1) {
  servoRight.writeMicroseconds(1700);        // Right wheel clockwise
  servoLeft.writeMicroseconds(1300);   
  }
  
  
