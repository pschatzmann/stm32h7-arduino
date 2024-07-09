/**
 * Just a short demo how to use the LED pin. 
 * We define our own constant but you maybe better use the LED_BUILTIN
 * which is provided by the board mapping.
 * @author Phil Schatzmann
 */

#define LED PA1

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}