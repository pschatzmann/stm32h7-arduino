// constants won't change. They're used here to set pin numbers:
const int buttonK1Pin = PE3;  // the number of the pushbutton pin
const int buttonK2Pin = PA1;  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin

// variables will change:
bool buttonState;  // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pins as an input:
  pinMode(buttonK1Pin, INPUT);
  pinMode(buttonK2Pin, INPUT);
}

void loop() {
  // read the state of the pushbutton value:
  bool buttonState1 = digitalRead(buttonK1Pin);
  bool buttonState2 = digitalRead(buttonK2Pin);
  buttonState = buttonState1 || buttonState2;

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
