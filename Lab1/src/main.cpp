/*--------------------------------------------------------------------
Name:   Demetri Papageorge
Date:   2/5/23  
Course: CSCE 336 Embedded Systems (Spring 2023) 
File:   main.cpp
HW/Lab: Lab 1

Purp: turn on LEDs and move a servo

Doc:  I worked alone

Academic Integrity Statement: I certify that, while others may have 
assisted me in brain storming, debugging and validating this program, 
the program itself is my own work. I understand that submitting code 
which is the work of other individuals is a violation of the honor   
code.  I also understand that if I knowingly give my original work to 
another individual is also a violation of the honor code.
--------------------------------------------------------------------*/
#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 90;    // variable to store the servo position

// Definitions for Port D registors
#define PIND_Reg (*((volatile uint8_t *) 0x29))
#define DDRD_Reg (*((volatile uint8_t *) 0x2a))
#define PORTD_Reg (*((volatile uint8_t *) 0x2b))
// Definitions for LED assignments:
#define RED_LED 5     //pin 5 is PortD bit 5  
#define GREEN_LED 6  //pin 6 is PortD bit 6
#define BLUE_LED 7   //pin 7 is PortD bit 7

#define BUTTON 4 

uint8_t loop_count = 0;
int lastnum = 1200;
int increment = 3;

void LEDInit(){
 //Set pinmode for LEDs to output
  DDRD_Reg |= (1 << RED_LED);
  DDRD_Reg |= (1 << GREEN_LED);
  DDRD_Reg |= (1 << BLUE_LED);

  //Turn all off
  PORTD_Reg &= ~(1 << RED_LED);   //clear output
  PORTD_Reg &= ~(1 << GREEN_LED); //clear output
  PORTD_Reg &= ~(1 << BLUE_LED);  //clear output
}

//checkoff 2
void LED_Checkoff(){
  PORTD_Reg |= (1 << RED_LED);    //set output
  delay(200);
  PORTD_Reg |= (1 << GREEN_LED);  //set output
  delay(200);
  PORTD_Reg &= ~(1 << RED_LED);   //clear output
  delay(200);
  PORTD_Reg |= (1 << BLUE_LED);   //set output
  delay(200);
  PORTD_Reg &= ~(1 << GREEN_LED); //clear output
  delay(200);
  PORTD_Reg |= (1 << RED_LED);    //set output
  delay(200);
  PORTD_Reg &= ~(1 << BLUE_LED);   //clear output
  PORTD_Reg &= ~(1 << RED_LED);   //clear output
  delay(600);
}

//checkoff 5
void ServoSweep()
{
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
   }                          // waits for the servo to get there
}

//checkoff 6
void TwoPhotoresistors()
{
  while ((pos > 0)|(pos < 180))
  {
    delay(30);
    if (analogRead(A0)>analogRead(A1))
    {
      pos -= 1;
    }
    else
    {
      pos += 1;
    }
    myservo.write(pos);
  }
}

//checkoff 7
void OnePhotoresistor()
{
  delay(20);
  if (analogRead(A0)<lastnum)
  {
    increment *= -1;
  }
  pos += increment;
  myservo.write(pos);
  lastnum = (analogRead(A0)-lastnum)/2+lastnum;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up...");
  myservo.attach(3);
  LEDInit();
  //Set pinmode for Button as input
  DDRD_Reg &= ~(1 << BUTTON);
  //Enable pullup 
  PORTD_Reg |= (1 << BUTTON);  //set output to enable pullup resistor
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (!(PIND & (1 << BUTTON))){
    Serial.println("Button Pressed");
    ServoSweep();
    LED_Checkoff();
    Serial.println(analogRead(A0));
  }
  delay(30);
  TwoPhotoresistors();
  OnePhotoresistor();
  Serial.print("loop ");
  Serial.print(loop_count);
  Serial.print(" complete\n");
  loop_count++;
}