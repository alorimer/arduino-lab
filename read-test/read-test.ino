int incomingByte = 0;   // for incoming serial data
String incomingText;

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
}
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
                incomingText.concat(Serial.readString());
                Serial.println("incomingText[] is currently: " + String(incomingText));
                
        }
}
