#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {0xFE, 0xAD,0x00, 0x00, 0x05, 0x00};
IPAddress ip(138, 194, 25, 55);
IPAddress myDns(8, 8, 8, 8);
char server[] = "arduino.lorimer.id.au";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
 // start the serial library:
 Serial.begin(9600);
 // start the Ethernet connection:
 if (Ethernet.begin(mac, ip, myDns) == 0) {
   Serial.println("Failed to configure Ethernet using DHCP");
   // no point in carrying on, so do nothing forevermore:
   for(;;)
     ;
 }
 // give the Ethernet shield a second to initialize:
 delay(1000);

}

void loop()
{
 Serial.println("connecting...");

 // if you get a connection, report back via serial:
 if (client.connect(server, 80)) {
   Serial.println("connected");
   // Make a HTTP request:
   client.println("GET /ledstatus.php HTTP/1.0");
   client.println();
   delay(1000);
   char c;
   // go trough the responce. as the value we are looking for is in the end of the responce after this loop char c will contain the last char of the respoce, 0 or 1
   while (client.available()) {
     c = client.read();
   }
 
   // if char c converted to an integer is somethning else than 0
   if (atoi(&c)) {
         Serial.println("Led should be on");
         //turn the led on here
       } 
       else{
          Serial.println("Led should be off");
          //turn the led off here
       } 
  
 } 
 else {
   // if you didn't get a connection to the server:
   Serial.println("connection failed");
 }
 // if the server's disconnected, stop the client:
 if (!client.connected()) {
   Serial.println("disconnecting.");
   client.stop();
 }
 delay(5000);
}
