/*  Last used 17/11/2015
    Michael Best    Ver 2.1b    16-07-2015
    ATtiny85_LDR_5-LED
    Arduino IDE 1.6.5

   http://www.mlbelanger.com/arduino/attiny/attiny85-via-arduino-quickstart-guide/

   http://highlowtech.org/?p=1695
   http://42bots.com/tutorials/how-to-program-attiny85-with-arduino-uno-part-1/
   http://42bots.com/tutorials/how-to-program-attiny85-with-arduino-uno-part-2/

   http://www.atmel.com/Images/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf
   
  Must install ATtiny support in Arduino 1.6.5 (http://highlowtech.org/?p=1695)
  In Arduino 1.6.5, you can install the ATtiny support using the built-in boards manager.
   * Open "File > Preferences" dialog in the Arduino software.
   * Find the “Additional Boards Manager URLs” field near the bottom of the dialog.
   * Paste the following URL into the field (use a comma to separate it from any URLs you’ve already added):
       https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json
   * Click the OK button to save your updated preferences.
   * Open the boards manager in the “Tools > Board > Board Manager” menu.
   * Scroll to the bottom of the list; you should see an entry for “ATtiny”.
   * Click on the ATtiny entry. An install button should appear. Click the install button.
   * The word “installed” should now appear next to the title of the ATtiny entry.
   * Close the boards manager. 
   * You should now see an entry for ATtiny in the “Tools > Board” menu.   

  Setup Arduino UNO as a programmer - "Arduino as ISP" <<<<  http://www.instructables.com/id/ATtiny-Programming-Shield-for-Arduino-1/
   a) Tools > Boards > Arduino UNO
   b) Tools > Serial Port > Comm??? (Ardiuno UNO)
   c) Program Arduino as ISP (Files > Examples > ArduinoISP)                                               

  ###########################################################################################################
  Arduino software to communicate to the ATtiny85 using "Arduino as ISP":
   3) Tools > Programmer > "Arduino as ISP"
   4) Tools > Boards > ATtiny
   5) Tools > Processor > ATtiny85
   6) Tools > Clock > 1MHz (internal clock)
   7) Tools > Serial Port > Comm??? (Ardiuno UNO)
   8) Sketch > Upload Using Programmer
  ------------ Message ------------
  Sketch uses 2,186 bytes (26%) of program storage space. Maximum is 8,192 bytes.
  Global variables use 36 bytes of dynamic memory.
  ------------
  ###########################################################################################################

   -or-  ??? Only worked once ???
   Arduino software to communicate to the ATtiny85 using "AVR ISP":             NOT using Sparkfun "AVR ISP" Shield with Arduino UNO (ArduinoISP 04m3, NO bootloader) https://www.sparkfun.com/products/11168
   3) Tools > Programmer > "AVR ISP"
   4) Tools > Boards > ATtiny85 (internal 1MHz clock) to select the ATtiny85 chip as our target.
   5) Tools > Serial Port > Comm??? (Ardiuno UNO)
   6) File > Upload Using Programmer

   Ignor Error >> avrdude: please define PAGEL and BS2 signals in the configuration file for part ATtiny85   http://highlowtech.org/?p=1229
   Note that stock ATtiny85 chips run at 1MHz speed so the 8MHz setting won't work out of the box.
*/

#define TRUE  1
#define FALSE 0

// ------ Program for Arduino UNO for debugging instead of ATtiny85 ------ 
// #define UNO_DEBUG

// ------- NOTE: Pin 1 is Analog inpot (A0) shared with RESET line.  Could do fuse to disable the RESET pin, but then can't reburn the chip without a High Voltage Programming.  ------
// ------- So ADC input (hardware) if <1V will RESET. -----
// ------- But if it does get to LOW it will reboot the ATtiny85 -----

// ------- Light Dependent Resistor (2K-200KR, Element14:2568243) with 100KR pull down = RESET will not activate, but has poor range ----
// #define ADC_MIN    0x350   // Dark Min Level 
// #define ADC_MAX    0x3F0   // Bright Max Level

// ------- Light Dependent Resistor (2K-200KR, Element14:2568243) with 10KR pull down = RESET will activate, but has better range----
#define ADC_MIN    0x300   // Dark Min Level 
#define ADC_MAX    0x370   // Bright Max Level


// 20ms to 0.5s
#define WAIT_MIN 20
#define WAIT_MAX 500



// ATMEL ATTINY85 / ARDUINO
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2)  Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+
#define SENSOR_PIN   A0     // Pin 1 = A0 (PB5); select the input pin for the LDR - Light Dependent Resistor (2K-200KR), Far 2568243
#define LED_1_PIN     3     // Pin 5 = D0 (PB0); select the outpu pin for the LED1
#define LED_2_PIN     4     // Pin 6 = D1 (PB1); select the outpu pin for the LED2
#define LED_3_PIN     0     // Pin 7 = D2 (PB2); select the outpu pin for the LED3
#define LED_4_PIN     1     // Pin 2 = D3 (PB3); select the outpu pin for the LED4
#define LED_5_PIN     2     // Pin 3 = D4 (PB4); select the outpu pin for the LED5


int ledCount = 5;
// int ledPins[] = { LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN, LED_5_PIN };
int ledPins[] = { 3, 4, 0, 1, 2 };

int AC_light;

unsigned int adc_min, adc_max;



void setup()
//=====================================================
{
  // declare the ledPin as an OUTPUT
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT); 
    }

  // declare the Sensor Pin as an INPUT
  pinMode(SENSOR_PIN, INPUT);

  // ------ Show that we have just booted up
  for(int i=0; i<10; i++) {
    // ------ LED Scroll UP ------
    for (int thisLed = 0; thisLed < ledCount; thisLed++) {
      digitalWrite(ledPins[thisLed], HIGH);
      delay(50);
      digitalWrite(ledPins[thisLed], LOW);
      } 
    }
   
  #ifdef UNO_DEBUG
    Serial.begin(9600);
  #endif  

  // ---- Pre setup default min/max ----
  //  adc_min = ADC_MIN;
  //  adc_max = ADC_MAX;

  // ---- Pre setup default min/max for auto-scaling ----
  adc_min = 0xFFFF;
  adc_max = 0x0000;
}





void loop()
//=====================================================
{
  int period = 0; 
  static int period_sum = 0;
  int bargraphValue;
  static byte SCROLL = TRUE;
  static byte bargraph_on = TRUE;
  int MAX_PERIOD = (5 * 1000);  //  5 seconds

  if(SCROLL) {
    // ------ Do LED scroll up / down ------
    for (int thisLed = 0; thisLed < ledCount-1; thisLed++) {
      // ------ Scroll up ------
      digitalWrite(ledPins[thisLed], HIGH);
      period = sensorValue();
      period_sum = period + period_sum;
      delay(period);
      digitalWrite(ledPins[thisLed], LOW);
      }
    for (int thisLed = ledCount-1; thisLed > 0; thisLed--) {
      // ------ Scroll down ------
      digitalWrite(ledPins[thisLed], HIGH);
      period = sensorValue();
      period_sum = period + period_sum;
      delay(period);
      digitalWrite(ledPins[thisLed], LOW);
      }
    if(period_sum > MAX_PERIOD) {
      period_sum = 0;
      SCROLL = FALSE;   
      }
    }  
    
  else {
    // ------ Do LED Bargraph ------
    static byte bargraph_on = TRUE;
    bargraphValue = map(sensorValue(), WAIT_MAX, WAIT_MIN, 0, 5);
    for (int thisLed = 0; thisLed < ledCount; thisLed++) {
      if(!AC_light || bargraph_on) {
        // ------ DC Lighting or AC Lighting flash bargraph on -----
        if(bargraphValue < thisLed)       digitalWrite(ledPins[thisLed], LOW);
        else                              digitalWrite(ledPins[thisLed], HIGH);
        }
      else {
        // ------ AC Lighting flash bargraph off ------
        digitalWrite(ledPins[thisLed], LOW);
        }
      }
    bargraph_on = bargraph_on ^ 1;    // For AC Lighting toggle bargraph On/Off 
    period = 50;
    period_sum = period + period_sum;
    delay(period);
    if(period_sum > MAX_PERIOD * 2) {
      period_sum = 0;
      SCROLL = TRUE;   
      }
    }
      
}



int sensorValue()
//=====================================================
{
  unsigned int sensorValue, result;
  unsigned int maxValue, minValue, diffValue;

  // --- read the ADC value from the sensor ---
  sensorValue = analogRead(SENSOR_PIN);

  // --- Burst ADC mode --- Check if light source is AC or DC ---
  maxValue = minValue = sensorValue;
  for(int i = 0; i < 50;  i++) {
    sensorValue = analogRead(SENSOR_PIN);
    if(sensorValue < minValue)   minValue = sensorValue;
    if(sensorValue > maxValue)   maxValue = sensorValue;
    }
  diffValue = maxValue - minValue;
  if(diffValue > 1)   AC_light = TRUE;
  else                AC_light = FALSE;  

  // --- Auto scale ---
  if(sensorValue < adc_min)   adc_min = sensorValue;
  if(sensorValue > adc_max) adc_max = sensorValue;

  // --- Constrain and map ADC to a delay value ---
  result = constrain(sensorValue, adc_min, adc_max);
  result = map(result, adc_min, adc_max, WAIT_MAX, WAIT_MIN);
  
  #ifdef UNO_DEBUG
    Serial.print("raw=");        Serial.print(sensorValue);     Serial.print(" 0x");         Serial.print(sensorValue, HEX);
    Serial.print(", min=0x");    Serial.print(adc_min, HEX);    Serial.print(", max=0x");    Serial.print(adc_max, HEX);    
    Serial.print(", diff=");     Serial.print(diffValue);    
    Serial.print(", now=");      Serial.print(result);
    if(AC_light) Serial.println(", AC Lighting"); 
    else         Serial.println(", DC Lighting"); 
  #endif

  return(result);    
}



