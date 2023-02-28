/*--------------------------------------------------------------------
Name: 	Demetri Papageorge
Date: 	2/13/23	
Course: CSCE 336 Embedded Systems (Spring 2023) 
File: 	main.cpp
HW/Lab:	Homework 2

Purp:	Blink LED with user input, have different length loops based on integer size

Doc:	I worked alone

Academic Integrity Statement: I certify that, while others may have 
assisted me in brain storming, debugging and validating this program, 
the program itself is my own work. I understand that submitting code 
which is the work of other individuals is a violation of the honor   
code.  I also understand that if I knowingly give my original work to 
another individual is also a violation of the honor code.
--------------------------------------------------------------------*/
#include <Arduino.h>

// Definitions for Port D registors
#define PIND_Reg (*((volatile uint8_t *) 0x29))
#define DDRD_Reg (*((volatile uint8_t *) 0x2a))
#define PORTD_Reg (*((volatile uint8_t *) 0x2b))
#define RED_LED 5 
#define s 0x43
#define f 0x36

int speed = 500;
uint8_t cntr8 = 0xff;
uint16_t cntr16 = 0xffff;
uint32_t cntr32 = 0xffffffff; 

void setup() {
  Serial.begin(9600);
  DDRD_Reg |= (1 << RED_LED);
  PORTD_Reg &= ~(1 << RED_LED);
}

//Blinks lights number of times based off inputted integer
void SerialLED(){
  int incomingbyte = 0;
  if (Serial.available() > 0){
    incomingbyte = Serial.read()-0x30;
  }
  //slows with s press, speeds up with f press
  if (incomingbyte == f){
    speed = 250;
  }
  else if (incomingbyte == s){
    speed = 1000;
  }
  //only allows inputs 0-9
  if (incomingbyte<=9 && incomingbyte>0){
    Serial.print(incomingbyte);
    for (int i = 0; i<incomingbyte;i++){
      PORTD_Reg |= (1 << RED_LED);    //set output
      delay(speed);
      PORTD_Reg &= ~(1 << RED_LED);   //clear output
      delay(speed);
    }
  }
}

//loop with 8bit integer
void eight(){
  PORTD_Reg |= (1 << RED_LED);
  for(cntr8 = 0; cntr8 < 0xff; cntr8++){ 
    asm volatile("nop"); 
  }
  PORTD_Reg &= ~(1 << RED_LED);
}
//loop with 16bit integer
void sixteen(){
  PORTD_Reg |= (1 << RED_LED);
  for(cntr16 = 0; cntr16 < 0xffff; cntr16++){ 
    asm volatile("nop"); 
  }
  PORTD_Reg &= ~(1 << RED_LED);
}
//loop with 32bit integer
void thirtytwo(){
  PORTD_Reg |= (1 << RED_LED);
  for(cntr32 = 0; cntr32 < 0x28FFFF; cntr32++){ 
    asm volatile("nop"); 
  }
  PORTD_Reg &= ~(1 << RED_LED);
}

//runs loops in correct sequence
void LEDloops(){
  delay(500);
  thirtytwo();
  delay(500);
  PORTD_Reg |= (1 << RED_LED);
  delay(500);
  PORTD_Reg &= ~(1 << RED_LED);
  delay(500);
  for (int i = 0; i<36;i++){
    sixteen();
  }
  delay(500);
  PORTD_Reg |= (1 << RED_LED);
  delay(500);
  PORTD_Reg &= ~(1 << RED_LED);
  delay(500);
  PORTD_Reg |= (1 << RED_LED);
  delay(500);
  PORTD_Reg &= ~(1 << RED_LED);
  delay(500);
  for (int i = 0; i<10350;i++){
    eight();
  }
}

void loop() {
  //SerialLED();
  LEDloops();
}

/*
Homework Questions
2b:
operating voltage = 1.8-5.5V
power = 1.8V(25mA) = .045W
power = 5.5V(25mA) = .1375W

2c:
20MHz clock

3a:
8bit;
5.799s/16M cycles * 255cycles/30000loop = 3.08*10^-9s per loop
16bit;
8.241s/16M cycles * 65536cycles/100loop = 3.37*10^-4s per loop
32bit;
19.487s/16M cycles * 7340032cycles/1loop = 8.939s per loop


4a:
pinMode(3,OUTPUT);
digitalRead(3);

4b:
PD3 Pin 4

4c:
PIND_Reg
PORTD_Reg
DDRD_Reg
DDRD_Reg &= ~(1 << 2);
(PIND_Reg & (1 << 2)>>2);

4d:
DDRD_Reg |= (1 << 2);
PIND_Reg &= ~(1 << 2);

5a:
5-1k(.5mA)-2k(.5mA)-3k(.5mA)=.5mAR4
R4 = 4000ohms

5b:
A = 5-1k(.5mA) = 4.5V
B = 4.5V - 2k(.5mA) = 3.5V
C = 3.5V - 3k(.5mA) = 2V

5c:
Series = 1k+2k+3k+4k = 10k ohms
Parallel = 1/(1/1k + 1/2k + 1/3k + 1/4k) = 480 ohms

6a:
PORTC memory address 0x28 offset by 0x08 higher

6b:
DDRC_Reg = ((DDRC_Reg & 0xF3) | ((var & 0x60) >> 3))

6c:
xchg instructions take 3 clock cycles

6d:
end address of SRAM is 0x08FF
Found in SRAM Data Memory
*/