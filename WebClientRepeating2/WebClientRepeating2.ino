/*
  Repeating Web client

 This sketch connects to a a web server and makes a request
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.

 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 19 Apr 2012
 by Tom Igoe
 modified 21 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.

 */

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 0xFE, 0xAD,0x00, 0x00, 0x05, 0x00 };
IPAddress ip(138, 194, 25, 55);
IPAddress myDns(8, 8, 8, 8);
char server[] = "arduino.lorimer.id.au";
//IPAddress server(117,55,235,26);

// initialize the library instance:
EthernetClient client;

//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers
int hash = 0;
void setup() {
  pinMode(5, OUTPUT);
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    
    char c = client.read();
    if (c == '#') {
      hash++;
      Serial.println(hash);
    }
      
      if (hash == 3 && c == '1') {
        digitalWrite(5,1);
        Serial.println("got 1");
        hash = 0;
      }
      if (hash == 3 && c == '0') {
        digitalWrite(5,0);
        Serial.println("got 0");
      hash = 0;
    }
   
    
    Serial.write(c);
  }
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
    
    client.println("Host: arduino.lorimer.id.au");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
Serial.println("value " + client.read());
   //digitalWrite(5, atoi(client.read()));

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
