#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xFE, 0xAD,0x00, 0x00, 0x05, 0x00 };
IPAddress ip(138, 194, 25, 55);
IPAddress myDns(8, 8, 8, 8);
char server[] = "arduino.lorimer.id.au";
//IPAddress server(117,55,235,26);

EthernetClient client;




unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  delay(50);
  // start serial port:

  pinMode(5, OUTPUT);

  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
}

void loop() {

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /ledstatus.php HTTP/1.1");
    client.print("Host: ");      client.println(server);
    client.println("User-Agent: arduino-ethernet");

    client.println();

    char c;
   // go trough the responce. as the value we are looking for is in the end of the responce after this loop char c will contain the last char of the respoce, 0 or 1
   digitalWrite(5, atoi(c));

    // note the time that the connection was made:
    lastConnectionTime = millis();
    digitalWrite(8, 1);
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    digitalWrite(8, 0);
  }
}
