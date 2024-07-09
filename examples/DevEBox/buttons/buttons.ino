/**
 * Just a short demo how to use the buttons
 * We define our own constant but you maybe better use the LED_BUILTIN,BTN_K1
 * and BTN_K2 which are provided by the board variant.
 * @author Phil Schatzmann
 */

// constants won't change. They're used here to set pin numbers:
const int buttonK1Pin = PE3;  // the number of the pushbutton pin
const int buttonK2Pin = PC5;  // the number of the pushbutton pin
const int ledPin = PA1;       // the number of the LED pin

// variables will change:
bool buttonPressed;  // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pins as an input: Not pressed is high!
  pinMode(buttonK1Pin, INPUT_PULLUP);
  pinMode(buttonK2Pin, INPUT_PULLUP);
}

void loop() {
  Serial.begin(115200);
  // read the state of the pushbutton value: active is low -> so we invert the
  // result
  bool buttonPressed1 = !digitalRead(buttonK1Pin);
  bool buttonPressed2 = !digitalRead(buttonK2Pin);
  Serial.print(buttonPressed1);
  Serial.print(" ");
  Serial.print(buttonPressed2);
  buttonPressed = buttonPressed1 || buttonPressed2;
  Serial.print(" -> ");
  Serial.println(buttonPressed);

  // check if the pushbutton is pressed. If it is, the buttonPressed is HIGH:
  if (buttonPressed) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}
