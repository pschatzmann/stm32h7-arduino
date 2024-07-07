
/***
 * Just print hallo world to serial:
 * Select
 * - USB Support: CDC Generic Serial supersedes USART
 * - USART Support: Enabled (generic Serial)
 * 
 */

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("Hallo world");
    delay(2000);
}