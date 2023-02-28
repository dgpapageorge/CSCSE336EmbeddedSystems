//Timer0 demo
#include <Arduino.h>

#define leftMotorPWN 5
#define leftMotorRed 7
#define leftMotorBlack 4

void setup() {
  Serial.begin(9600);
  Serial.print("Starting up");

  DDRD |= (1 << leftMotorBlack);
  DDRD |= (1 << leftMotorPWN);
  DDRD |= (1 << leftMotorRed);

  PORTD &= ~(1 << leftMotorBlack);
  PORTD &= ~(1 << leftMotorPWN);
  PORTD &= ~(1 << leftMotorRed);

  TCCR0A = ((1 << COM0B1) | (1 << WGM01) | (1 << WGM00));
  TCCR0B = (1 << CS02);
  OCR0B = 0x80; //50%
}

void loop() {
  Serial.println("Motors turning backwards at 50%");
  PORTD |= (1 << leftMotorBlack);  //turn in one direction
  PORTD &= ~(1 << leftMotorRed);
  OCR0B = 0x80; //50%

  _delay_ms(1000); //get ms without using timer0
  //Delay(100); this breaks the code since it uses timer0

  Serial.println("Motors turning forwards at 50%");
  PORTD &= ~(1 << leftMotorBlack);
  PORTD |= (1 << leftMotorRed);
  OCR0B = 0x80; //50%

  _delay_ms(1000);

  Serial.println("Motors turning forwards at 25%");
  OCR0B = 0x40; //25%
}