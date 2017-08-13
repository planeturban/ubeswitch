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
#define switchTime 500000
#define saveTime 2000000
#define bootWait 4000

unsigned long buttonPressedTime;
//int bootWait = 4000000;
//unsigned long pulseThreshold = 220;
int pulseThreshold = 15000;


byte state;
byte lastState;


unsigned long pulse = 0;

void setup() {
  PORTB |= bit(buttonPin); // Input pullup.
  DDRB |= bit(selectPin);
  state = EEPROM.read(0);
  bitWrite(PORTB, selectPin, state);
  _delay_ms(bootWait); // Wait for computer to stabilize, 3s seems ok.
  lastState = state;
}


void loop() {

  /*
     Detection..
  */
  pulse = 0;
  while ( ! pulse)
    pulse = pulseIn(pulsePin, HIGH, 1000000L);
  state = pulse < pulseThreshold;

  if ( state != lastState )
    bitWrite(PORTB, selectPin, state);
  lastState = state;

  /*
       Button, save and switch.
  */

  if ( !bitRead(PINB, buttonPin) ) {
    buttonPressedTime = micros();
    while ( ! bitRead(PINB, buttonPin)); // Ugly debounce
    if ( micros() > saveTime  + buttonPressedTime ) {
      EEPROM.write(0, state);
    } else if ( micros() > buttonPressedTime + switchTime ) {
      PORTB ^= bit(selectPin);
      _delay_ms(1000); // Wait for computer to reboot and get its shit togheter.
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






