#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial); // GPS object called GPS
#define GPSECHO  false // raw sentences

float latLine = -37.906877;
float lonLine;

boolean usingInterrupt = true;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

void setup() {

  // initialisation
  Serial.begin(115200);
  GPS.begin(9600);

  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // minimum output + GGA (fix data)
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // minimum output
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  for(int i=5; i<=8; i++) { pinMode(i, OUTPUT); } // set led pins

  // print init
  Serial.println("\n --");
  Serial.println("<?xml version='1.0' encoding='UTF-8'?>");
  Serial.println("<gpx version='1.0'>");
  Serial.println("  <name>test file</name>");
  Serial.println("  <trk><name>test track</name><number>1</number><trkseg>");

  delay(1000);
}





// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) { GPS.read();}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();





void loop() {
  if (GPS.newNMEAreceived()) { // checksum and parse
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }
  
  if (millis() - timer > 1000) { 
    timer = millis(); // reset the timer
    if (GPS.fix) {
      Serial.println("    <trkpt lat='" + String(GPS.latitudeDegrees, 6) + "' lon='" + String(GPS.longitudeDegrees, 6) + "'></trkpt>");
       digitalWrite(8, LOW);
      if ((latLine - 0.000100) < GPS.latitudeDegrees < latLine + 0.000010) {
        digitalWrite(5, LOW);
        digitalWrite(6, HIGH);
        digitalWrite(7, LOW);
      }
      else if (GPS.latitudeDegrees < latLine - 0.000010) {
        digitalWrite(5, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
      }
      else if (GPS.latitudeDegrees > latLine + 0.000010) {
         digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, HIGH);
      }
      
    }
    else {
      digitalWrite(8, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
    }
  }
}
