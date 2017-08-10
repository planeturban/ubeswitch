/*
   Compiled with MicroCore: 562 bytes.
   Compiled for Attiny85: 964 bytes.
*/

#include <EEPROM.h>



//#define ATTINY13

//#define doSamples

#define buttonPin PB0       // Connected to switch
#define selectPin PB1     // TS5V330 pin 1
#define pulsePin PB2       // VSync
#define switchTime 500
#define saveTime 2000

unsigned long buttonPressedTime;
int pulseRead = 0;
//unsigned long pulseThreshold = 220;
int pulseThreshold = 15000;


bool state;
bool lastState;


unsigned long pulse = 0;

void setup() {
  PORTB |= bit(buttonPin); // Input pullup.
  DDRB |= bit(selectPin);
  state = EEPROM.read(0);
  bitWrite(PORTB, selectPin, state);
  delay(2000);
  lastState = state;
}


void loop() {

  /*
     Detection..
  */



  state = pulseIn(pulsePin, HIGH, 1000000L);  // Ugly, but it saved 30 bytes over "state = pulseIn(pulsePin, HIGH, 1000000L) < pulseThreshold;"
  state = state < pulseThreshold;

  if ( state != lastState )
    bitWrite(PORTB, selectPin, state);
  lastState = state;

  /*
       Button, save and switch.
  */
  if ( !bitRead(PINB, buttonPin) ) {
    buttonPressedTime = millis();
    while ( ! bitRead(PINB, buttonPin)); // Ugly debounce
    if ( millis() > saveTime  + buttonPressedTime ) {
#ifdef __AVR_ATtiny13__
      EEPROM.update(0, state);
#else
      EEPROM.write(0, state);
#endif


    } else if ( millis() > buttonPressedTime + switchTime ) {
      PORTB ^= bit(selectPin);
      delay(1000);
    }
  }
}

unsigned long getPulse() {
/* Works with atmega... */
  
  pulse = bitRead(PINB, pulsePin);
  while ( bitRead(PINB, pulsePin) == pulse);
  unsigned long pstart = micros();
  while ( bitRead(PINB, pulsePin) != pulse);
  return ( micros() - pstart);

}






