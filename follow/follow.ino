#include <SoftwareSerial.h>
#include "Adafruit_GPS.h"

#ifdef __AVR_ATmega328P__
  SoftwareSerial mySerial(3, 2);
#elif defined(__AVR_ATtiny85__)
  SoftwareSerial mySerial(3, 4);
#else
  #error "invalid board.. must be Uno ISP or ATtiny85"
#endif

Adafruit_GPS GPS(&mySerial); // GPS object called GPS
#define GPSECHO  false // raw sentences

float latLine = -37.906877;

int nthPin, sthPin, centrePin;

boolean usingInterrupt = true;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

///////////////////////////////////
        
void setup() {

  // initialisation

  #ifdef __AVR_ATmega328P__
    Serial.begin(115200);
    sthPin = 5;
    centrePin = 6;
    nthPin = 7;
    //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // minimum output + GGA (fix data)
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // minimum output
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

    // print init
    Serial.println("\n --");
    Serial.println("<?xml version='1.0' encoding='UTF-8'?>");
    Serial.println("<gpx version='1.0'>");
    Serial.println("  <name>test file</name>");
    Serial.println("  <trk><name>test track</name><number>1</number><trkseg>");
  #elif defined(__AVR_ATtiny85__)
    sthPin = 0;
    centrePin = 1;
    nthPin = 2;
  #else
    Incompatible processor!
  #endif

  pinMode(sthPin, OUTPUT);
  pinMode(centrePin, OUTPUT);
  pinMode(nthPin, OUTPUT);
  
  GPS.begin(9600);

  

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  

  delay(1000);
}





// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) { GPS.read();}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    #ifdef __AVR_ATmega328P__
      TIMSK0 |= _BV(OCIE0A);
    #elif defined(__AVR_ATtiny85__)
      TIMSK |= _BV(OCIE0A);
    #endif
    
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    #ifdef __AVR_ATmega328P__
      TIMSK0 &= ~_BV(OCIE0A);
    #elif defined(__AVR_ATtiny85__)
      TIMSK &= ~_BV(OCIE0A);
    #endif
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
      #ifdef __AVR_ATmega328P__
        Serial.println("    <trkpt lat='" + String(GPS.latitudeDegrees, 6) + "' lon='" + String(GPS.longitudeDegrees, 6) + "'></trkpt>"); 
      #endif
      if (latLine == GPS.latitudeDegrees) {
        
        digitalWrite(sthPin, LOW);
        digitalWrite(centrePin, HIGH);
        digitalWrite(nthPin, LOW);
      }
      else if (GPS.latitudeDegrees < latLine) {
        digitalWrite(sthPin, HIGH);
        digitalWrite(centrePin, LOW);
        digitalWrite(nthPin, LOW);
      }
      else if (GPS.latitudeDegrees > latLine) {
         digitalWrite(sthPin, LOW);
        digitalWrite(centrePin, LOW);
        digitalWrite(nthPin, HIGH);
      }
      
    }
    else {
      digitalWrite(sthPin, LOW);
      digitalWrite(centrePin, LOW);
      digitalWrite(nthPin, LOW);
    }
  }
}
