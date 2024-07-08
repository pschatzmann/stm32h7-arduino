
/***
 * Just print hallo world to serial:
 * Select
 * - USB Support: CDC Generic Serial supersedes USART
 * - USART Support: Enabled (generic Serial)
 * 
 * After you deplayed your code and you still do not see any output, double check that
 * you selected the right output port in the Tools menu.
 */

void setup() {
    Serial.begin(115200);

    // wait for Serial to be ready
    while(!Serial);
}

void loop() {
    Serial.println("Hallo world");
    delay(2000);
}