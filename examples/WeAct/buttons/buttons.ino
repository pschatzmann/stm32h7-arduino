// constants won't change. They're used here to set pin numbers:
const int buttonPin = PC13;  // the number of the pushbutton pin
const int ledPin = PE3;    // the number of the LED pin

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pins as an input: Not pressed is high!
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  Serial.begin(115200);

  // check if the pushbutton is pressed. If it is, the buttonPressed is LOW:
  if (!digitalRead(buttonPin)) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
