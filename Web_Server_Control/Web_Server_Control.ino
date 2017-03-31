//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1284844153/0#0
//
//    WORK  "arduin0-55"  "138.194.25.55"
//    http://138.194.25.55/analogRead/0              <- To read analog0 value
//    http://138.194.25.55/digitalWrite/7/1          <- To turn on  LED on pin 8
//    http://138.194.25.55/digitalWrite/7/0          <- To turn off LED on pin 8
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 Arduino Data Web Server

 Performs actions on Arduino and/or returns data from Arduino to a webbrowser request URL:
  - To turn on LED on pin 8, use web browser to visit your Arduino via: http://x.x.x.x/digitalWrite/7/1
  - To turn it off send: http://x.x.x.x/digitalWrite/7/0
  - To read analog0 value, send: http://x.x.x.x/analogRead/0

 IMPORTANT:
 Commands are case sensitive, but I wrote this as simple as possible so it can be easily adapted to your needs.  
 Have fun!

 created Sept 17, 2010
 by Hari Wiguna, g33k.blogspot.com
*/

#include <SPI.h>
#include <Ethernet.h>
#include <string.h>


/**********************************************************************************************************************
*                                   MAC address and IP address.
***********************************************************************************************************************/
byte mac[] = {0xFE, 0xAD, 0x00, 0x00, 0x05, 0x00 };        // WORK  "arduin0-55"  "138.194.25.55"
byte ip[] = {138, 194, 25, 55};                            // WORK  "arduin0-55"  "138.194.25.55"

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

//EthernetServer server(80);


#define bufferMax 128
int bufferSize;
char buffer[bufferMax];


//-- Commands and parameters (sent by browser) --
char cmd[15];    // Nothing magical about 15, increase these if you need longer commands/parameters
char param1[15];
char param2[15];




void setup()
//===========================================================================================================================================
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at http://");
  Serial.print(Ethernet.localIP());
  Serial.println("/  and now waiting for a connection.");

  Serial.print("  http://");
  Serial.print(Ethernet.localIP());
  Serial.println("/digitalWrite/7/1          <- To turn ON  LED on pin 7");

  Serial.print("  http://");
  Serial.print(Ethernet.localIP());
  Serial.println("/digitalWrite/7/0          <- To turn OFF LED on pin 7");
  
  
  Serial.print("  http://");
  Serial.print(Ethernet.localIP());
  Serial.println("/analogRead/0              <- To read analog0 value");
  
  SetupSamplePorts();
}



void loop()
//===========================================================================================================================================
{
  EthernetClient client = server.available();
  if (client)
  {
    WaitForRequest(client);
    ParseReceivedRequest();
    PerformRequestedCommands();
    
    client.stop();
  }
}


//-- Sample Ports ---
void SetupSamplePorts()
//===========================================================================================================================================
{
/*
  // To illustrate how to use this, I have an LED and a Potentiometer.
  // The 10K potentiometer left lead is connected to GND, right lead to +5V, and middle lead to Analog 0.
  // The LED cathode is on digital pin 7 and anode is on pin 8.
  pinMode(7,OUTPUT); digitalWrite(7,LOW);  // I use this pin as GND for the LED.
  pinMode(8,OUTPUT); // Sample output, unable to use built-in LED at pin 13 because Ethernet Shield uses pins 10,11,12,13.
*/

  pinMode(5, OUTPUT);    // Green LED.
  pinMode(6, OUTPUT);    // Green LED.
  pinMode(7, OUTPUT);    // Red LED.

}



void WaitForRequest(EthernetClient client) // Sets buffer[] and bufferSize
//===========================================================================================================================================
{
  bufferSize = 0;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n')
        break;
      else
        if (bufferSize < bufferMax)
          buffer[bufferSize++] = c;
        else
          break;
    }
  }
  
  PrintNumber("bufferSize", bufferSize);
}




void ParseReceivedRequest()
//===========================================================================================================================================
{
  Serial.println("in ParseReceivedRequest");
  Serial.println(buffer);
  
  //Received buffer contains "GET /cmd/param1/param2 HTTP/1.1".  Break it up.
  char* slash1;
  char* slash2;
  char* slash3;
  char* space2;
  
  slash1 = strstr(buffer, "/") + 1; // Look for first slash
  slash2 = strstr(slash1, "/") + 1; // second slash
  slash3 = strstr(slash2, "/") + 1; // third slash
  space2 = strstr(slash2, " ") + 1; // space after second slash (in case there is no third slash)
  if (slash3 > space2) slash3=slash2;

  PrintString("slash1",slash1);
  PrintString("slash2",slash2);
  PrintString("slash3",slash3);
  PrintString("space2",space2);
  
  // strncpy does not automatically add terminating zero, but strncat does! So start with blank string and concatenate.
  cmd[0] = 0;
  param1[0] = 0;
  param2[0] = 0;
  strncat(cmd, slash1, slash2-slash1-1);
  strncat(param1, slash2, slash3-slash2-1);
  strncat(param2, slash3, space2-slash3-1);
  
  PrintString("cmd",cmd);
  PrintString("param1",param1);
  PrintString("param2",param2);
}




void PerformRequestedCommands()
//===========================================================================================================================================
{
  if ( strcmp(cmd,"digitalWrite") == 0 ) RemoteDigitalWrite();
  if ( strcmp(cmd,"analogRead") == 0 ) RemoteAnalogRead();
}




void RemoteDigitalWrite()
//===========================================================================================================================================
{
  int ledPin = param1[0] - '0'; // Param1 should be one digit port
  int ledState = param2[0] - '0'; // Param2 should be either 1 or 0
  digitalWrite(ledPin, ledState);

  //-- Send response back to browser --
  server.print("D");
  server.print(ledPin, DEC);
  server.print(" is ");
  server.print( (ledState==1) ? "ON" : "off" );

  //-- Send debug message to serial port --
  Serial.println("RemoteDigitalWrite");
  PrintNumber("ledPin", ledPin);
  PrintNumber("ledState", ledState);
}




void RemoteAnalogRead()
//===========================================================================================================================================
{
  // If desired, use more server.print() to send http header instead of just sending the analog value.
  int analogPin = param1[0] - '0'; // Param1 should be one digit analog port
  int analogValue = analogRead(analogPin);
  
  //-- Send response back to browser --
  server.print("A");
  server.print(analogPin, DEC);
  server.print(" is ");
  server.print(analogValue,DEC);
  
  //-- Send debug message to serial port --
  Serial.println("RemoteAnalogRead");
  PrintNumber("analogPin", analogPin);
  PrintNumber("analogValue", analogValue);
}



void PrintString(char* label, char* str)
//===========================================================================================================================================
{
  Serial.print(label);
  Serial.print("=");
  Serial.println(str);
}




void PrintNumber(char* label, int number)
//===========================================================================================================================================
{
  Serial.print(label);
  Serial.print("=");
  Serial.println(number, DEC);
} 

