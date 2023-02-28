/*--------------------------------------------------------------------
Name:   Demetri Papageorge
Date:   2/20/23  
Course: CSCE 336 Embedded Systems (Spring 2023) 
File:   main.cpp
HW/Lab: Lab 2, Button Count Code

Purp: A brief description of what this program does and 
    the general solution strategy. 

Doc:  no one helped me

Academic Integrity Statement: I certify that, while others may have 
assisted me in brain storming, debugging and validating this program, 
the program itself is my own work. I understand that submitting code 
which is the work of other individuals is a violation of the honor   
code.  I also understand that if I knowingly give my original work to 
another individual is also a violation of the honor code.
--------------------------------------------------------------------*/

#include <Arduino.h>
//#include <avr/io.h>

// Definitions for Port B & D registors
#define PINB_Reg (*((volatile uint8_t *) 0x23))
#define DDRB_Reg (*((volatile uint8_t *) 0x24))
#define PORTB_Reg (*((volatile uint8_t *) 0x25))
#define PIND_Reg (*((volatile uint8_t *) 0x29))
#define DDRD_Reg (*((volatile uint8_t *) 0x2a))
#define PORTD_Reg (*((volatile uint8_t *) 0x2b))

#define BUTTON 5 

#define ICP1 0

uint16_t regValue = TCNT1;
int buttonEndTime = 0;
int buttonStartTime = 0;
int pressed = 0;

void setup() {                
  Serial.begin(9600);
  Serial.println("Starting up...");
  //Set pinmode for Button as input
  DDRD_Reg &= ~(1 << BUTTON);
  //Enable pullup 
  PORTD_Reg |= (1 << BUTTON);  //set output to enable pullup resistor

  DDRB_Reg &= ~(1 << 0);
  PORTB_Reg |= (1 << 0);
  
  //Init counter1
  TCCR1A = 0; //Normal mode 0xffff top, rolls over
  
  //TCCR1B = (1 << CS12) | (1 << CS11); //checkoff 1
  //TCCR1B = (1 << CS12) | (1 << CS10); //checkoff 3
  TCCR1B = (1 << CS11) | (1 << CS11); //checkoff 5
  
  TCCR1C = 0;
  //Set counter to zero, high byte first
  TCNT1H = 0;
  TCNT1L = 0;  
  //Make sure interrupts are disabled 
  TIMSK1 = 0;
  TIFR1 = 0;

  
  Serial.println("Finished setup!");
}

//checkoff 1
void part1(){
  //counts button presses
  if (regValue != TCNT1){
    Serial.print("TCNT1: ");
    Serial.println(TCNT1);
    regValue = TCNT1;
  }
  delay(100);
}

//checkoff 3
void part3(){
  TCCR1B |= (1 << ICES1);
  //checks when button pressed
  if (!(PINB & (1 << ICP1))){
    TCNT1 = 0;
    ICR1 = 0;
    buttonStartTime = ICR1;
    delay(10);
    regValue = ICR1;
    //checks if bounce occurred
    if (regValue != 0){
      buttonEndTime = ICR1;
      Serial.print("Time since last press: ");
      Serial.println(buttonEndTime-buttonStartTime);
    }
  }
}

//checkoff 4
void part4(){
  TCCR1B &= ~(1 << ICES1);
  //checks when button pressed
  if (!(PINB & (1 << ICP1))){
    TCNT1 = 0;
    ICR1 = 0;
    //checks when button unpressed
    if ((PINB & (1 << ICP1))){
      buttonStartTime = ICR1;
      delay(10);
      regValue = ICR1;
      //checks if bounce occurred
      if (regValue != 0){
        buttonEndTime = ICR1;
        Serial.print("Bounce time: ");
        Serial.println(buttonEndTime-buttonStartTime);
      }
    }
  }
}

//checkoff 5
void part5(){
  TCCR1B &= ~(1 << ICES1);
  //checks if button pressed
  if (!(PINB & (1 << ICP1)) && pressed == 0){
    buttonStartTime = ICR1;
    pressed = 1;
  }
  //checks if button unpressed
  if ((PINB & (1 << ICP1)) && pressed == 1){
    buttonEndTime = ICR1;
    Serial.print("Time held: ");
    Serial.println(buttonEndTime-buttonStartTime);
    pressed = 0;
  }
}

void loop() {
  //part1();
  //part3();
  //part4();
  part5();
}
